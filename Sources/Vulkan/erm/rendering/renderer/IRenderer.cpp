#include "erm/rendering/renderer/IRenderer.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/ISwapChainListener.h"
#include "erm/rendering/data_structs/QueueFamilyIndices.h"
#include "erm/rendering/data_structs/SwapChainSupportDetails.h"
#include "erm/rendering/textures/CubeMap.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

IRenderer::IRenderer(Engine& engine)
	: mEngine(engine)
	, mWindow(engine.GetWindow())
	, mDevice(engine.GetDevice())
	, mResourcesManager(engine.GetResourcesManager())
	, mCurrentFrame(0)
	, mCurrentImageIndex(0)
	, mMinImageCount(0)
	, mImageCount(0)
	, mIsImageIndexValid(false)
	, mFramebufferResized(true)
{
	CreateSwapChain();
	CreateFrameResources();
	CreateDepthResources();
	CreateTextureSampler();
	CreateSyncObjects();
}

IRenderer::~IRenderer()
{
	mDevice->waitIdle();

	for (ISwapChainListener* listener : mSwapChainListeners)
	{
		listener->SwapChainCleanup();
	}

	CleanupSwapChain();

	mDevice->destroySampler(mTextureSampler);

	for (uint32_t i = 0; i < kMaxFramesInFlight; ++i)
	{
		mDevice->destroySemaphore(mRenderFinishedSemaphores[i]);
		mDevice->destroySemaphore(mImageAvailableSemaphores[i]);
		mDevice->destroyFence(mInFlightFences[i]);
	}
}

void IRenderer::AddSwapChainListener(ISwapChainListener* listener)
{
	mSwapChainListeners.insert(listener);
}

void IRenderer::RemoveSwapChainListener(ISwapChainListener* listener)
{
	auto it = mSwapChainListeners.find(listener);
	if (it != mSwapChainListeners.end())
	{
		mSwapChainListeners.erase(it);
	}
}

void IRenderer::RecreateSwapChain()
{
	mDevice->waitIdle();

	for (ISwapChainListener* listener : mSwapChainListeners)
	{
		listener->SwapChainCleanup();
	}

	CleanupSwapChain();

	CreateSwapChain();
	CreateFrameResources();
	CreateDepthResources();

	for (ISwapChainListener* listener : mSwapChainListeners)
	{
		listener->SwapChainCreated();
	}
}

void IRenderer::CleanupSwapChain()
{
	for (auto& [key, value] : mFrameBuffers)
	{
		for (Texture* texture : value)
		{
			// The present buffer images gets handled internally from the block chain, no need to delete them manually
			if (key == FrameBufferType::PRESENT)
				texture->mImage = nullptr;

			mResourcesManager.ReleaseTexture(texture);
		}
	}

	mFrameBuffers.clear();
	mDevice->destroySwapchainKHR(mSwapChain);
}

void IRenderer::CreateSwapChain()
{
	const math::vec<2, int>& frameBufferSize = mWindow.GetFrameBufferSize();

	SwapChainSupportDetails swapChainSupport = VkUtils::QuerySwapChainSupport(mDevice.GetVkPhysicalDevice(), mDevice.GetVkSurface());
	QueueFamilyIndices indices = VkUtils::FindQueueFamilies(mDevice.GetVkPhysicalDevice(), mDevice.GetVkSurface());

	vk::SurfaceFormatKHR surfaceFormat = VkUtils::ChooseSwapSurfaceFormat(swapChainSupport.mFormats);
	vk::PresentModeKHR presentMode = VkUtils::ChooseSwapPresentMode(swapChainSupport.mPresentModes);
	mSwapChainExtent = VkUtils::ChooseSwapExtent(swapChainSupport.mCapabilities, frameBufferSize.x, frameBufferSize.y);
	mSwapChainImageFormat = surfaceFormat.format;

	mMinImageCount = swapChainSupport.mCapabilities.minImageCount;
	mImageCount = mMinImageCount + 1;
	if (swapChainSupport.mCapabilities.maxImageCount > 0 && mImageCount > swapChainSupport.mCapabilities.maxImageCount)
	{
		mImageCount = swapChainSupport.mCapabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.surface = mDevice.GetVkSurface();
	swapChainCreateInfo.minImageCount = mImageCount;
	swapChainCreateInfo.imageFormat = mSwapChainImageFormat;
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent = mSwapChainExtent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment
#ifdef ERM_RAY_TRACING_ENABLED
		| vk::ImageUsageFlagBits::eStorage
#endif
		;

	if (indices.mGraphicsFamily != indices.mPresentFamily)
	{
		uint32_t queueFamilyIndices[] = {indices.mGraphicsFamily.value(), indices.mPresentFamily.value()};

		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	swapChainCreateInfo.preTransform = swapChainSupport.mCapabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapChainCreateInfo.presentMode = presentMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = nullptr;

	mSwapChain = mDevice->createSwapchainKHR(swapChainCreateInfo);
	mSwapChainImages = mDevice->getSwapchainImagesKHR(mSwapChain);

	mSwapChainImageViews.resize(mSwapChainImages.size());
	mFrameBuffers[FrameBufferType::PRESENT].resize(mSwapChainImages.size());

	for (size_t i = 0; i < mSwapChainImages.size(); ++i)
	{
		// Create image view
		vk::ImageViewCreateInfo viewInfo {};
		viewInfo.image = mSwapChainImages[i];
		viewInfo.viewType = vk::ImageViewType::e2D;
		viewInfo.format = mSwapChainImageFormat;
		viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		mSwapChainImageViews[i] = VkUtils::CreateImageView(
			mDevice.GetVkDevice(),
			viewInfo);

		// Create texture associated
		Texture* texture = mResourcesManager.CreateEmptyTexture();
		texture->InitWithData(
			("present_buffer_" + std::to_string(i)).c_str(),
			mSwapChainImages[i],
			mSwapChainImageViews[i],
			nullptr,
			vk::ImageLayout::ePresentSrcKHR,
			mSwapChainImageFormat,
			mSwapChainExtent.width,
			mSwapChainExtent.height);

		mFrameBuffers[FrameBufferType::PRESENT][i] = texture;
	}
}

void IRenderer::CreateFrameResources()
{
	static const std::array sBuffersToCreate {FrameBufferType::FRAME_1};

	for (size_t i = 0; i < sBuffersToCreate.size(); ++i)
	{
		mFrameBuffers[sBuffersToCreate[i]].resize(IRenderer::kMaxFramesInFlight);
		for (size_t j = 0; j < IRenderer::kMaxFramesInFlight; ++j)
		{
			vk::Format format = vk::Format::eB8G8R8A8Unorm;
			vk::Image image;
			vk::ImageView imageView;
			vk::DeviceMemory imageMemory;

			vk::ImageCreateInfo imageInfo {};
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.extent.width = mSwapChainExtent.width;
			imageInfo.extent.height = mSwapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = vk::ImageTiling::eOptimal;
			imageInfo.initialLayout = vk::ImageLayout::eUndefined;
			imageInfo.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
			imageInfo.samples = vk::SampleCountFlagBits::e1;
			imageInfo.sharingMode = vk::SharingMode::eExclusive;

			VkUtils::CreateImage(
				mDevice.GetVkPhysicalDevice(),
				mDevice.GetVkDevice(),
				imageInfo,
				image,
				imageMemory,
				MemoryProperty::DEVICE_LOCAL);

			vk::ImageViewCreateInfo viewInfo {};
			viewInfo.image = image;
			viewInfo.viewType = vk::ImageViewType::e2D;
			viewInfo.format = format;
			viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			imageView = VkUtils::CreateImageView(
				mDevice.GetVkDevice(),
				viewInfo);

			Texture* texture = mResourcesManager.CreateEmptyTexture();
			texture->InitWithData(
				("frame_buffer_1_" + std::to_string(i)).c_str(),
				image,
				imageView,
				imageMemory,
				vk::ImageLayout::eGeneral,
				format,
				mSwapChainExtent.width,
				mSwapChainExtent.height);

			mFrameBuffers[sBuffersToCreate[i]][j] = texture;
		}
	}
}

void IRenderer::CreateDepthResources()
{
	vk::Format depthFormat = VkUtils::FindDepthFormat(mDevice.GetVkPhysicalDevice());
	vk::Image depthImage;
	vk::ImageView depthImageView;
	vk::DeviceMemory depthImageMemory;

	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = mSwapChainExtent.width;
	imageInfo.extent.height = mSwapChainExtent.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = depthFormat;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;

	VkUtils::CreateImage(
		mDevice.GetVkPhysicalDevice(),
		mDevice.GetVkDevice(),
		imageInfo,
		depthImage,
		depthImageMemory,
		MemoryProperty::DEVICE_LOCAL);

	vk::ImageViewCreateInfo viewInfo {};
	viewInfo.image = depthImage;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.format = depthFormat;
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	depthImageView = VkUtils::CreateImageView(
		mDevice.GetVkDevice(),
		viewInfo);

	Texture* texture = mResourcesManager.CreateEmptyTexture();
	texture->InitWithData(
		"depth_buffer",
		depthImage,
		depthImageView,
		depthImageMemory,
		vk::ImageLayout::eGeneral,
		depthFormat,
		mSwapChainExtent.width,
		mSwapChainExtent.height);

	mFrameBuffers[FrameBufferType::DEPTH].emplace_back(texture);
}

void IRenderer::CreateTextureSampler()
{
	vk::SamplerCreateInfo samplerInfo {};
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.minFilter = vk::Filter::eLinear;
	samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16.0f;
	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = vk::CompareOp::eAlways;
	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	mTextureSampler = mDevice->createSampler(samplerInfo);
}

void IRenderer::CreateSyncObjects()
{
	mImagesInFlight.resize(mSwapChainImages.size(), nullptr);

	vk::SemaphoreCreateInfo semaphoreInfo = {};

	vk::FenceCreateInfo fenceInfo = {};
	fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

	for (uint32_t i = 0; i < kMaxFramesInFlight; ++i)
	{
		mImageAvailableSemaphores[i] = mDevice->createSemaphore(semaphoreInfo);
		mRenderFinishedSemaphores[i] = mDevice->createSemaphore(semaphoreInfo);
		mInFlightFences[i] = mDevice->createFence(fenceInfo);
	}
}

const std::vector<Texture*>& IRenderer::GetTargetFrameBuffers(FrameBufferType frameBufferType) const
{
	return mFrameBuffers.at(frameBufferType);
}

Texture* IRenderer::GetDefaultTexture(TextureType type) const
{
	switch (type)
	{
		case TextureType::DIFFUSE:
			return mResourcesManager.GetOrCreateTexture("res/textures/viking_room.png");
		case TextureType::NORMAL:
			return mResourcesManager.GetOrCreateTexture("res/textures/viking_room.png");
		case TextureType::SPECULAR:
			return mResourcesManager.GetOrCreateTexture("res/textures/viking_room.png");
		case TextureType::CUBE_MAP:
			return mResourcesManager.GetOrCreateCubeMap("res/cube_maps/skybox");
		default:
			ERM_ASSERT(false);
			return {};
	}
}

} // namespace erm
