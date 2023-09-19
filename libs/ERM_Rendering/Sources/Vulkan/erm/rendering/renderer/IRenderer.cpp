#include "erm/rendering/renderer/IRenderer.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/renderer/ISwapChainListener.h"
#include "erm/rendering/data_structs/QueueFamilyIndices.h"
#include "erm/rendering/data_structs/SwapChainSupportDetails.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/window/Window.h>

#include <erm/utils/Utils.h>

namespace erm {

IRenderer::IRenderer(
	Window& window,
	Device& device)
	: mWindow(window)
	, mDevice(device)
	, mCurrentFrame(0)
	, mCurrentImageIndex(0)
	, mMinImageCount(0)
	, mImageCount(0)
	, mIsImageIndexValid(false)
	, mFramebufferResized(true)
{
	createSwapChain();
	createFrameResources();
	createDepthResources();
	createTextureSampler();
	createSyncObjects();
}

IRenderer::~IRenderer()
{
	ERM_VK_CHECK(mDevice->waitIdle());

	for (auto* listener : mSwapChainListeners)
	{
		listener->swapChainCleanup();
	}

	cleanupSwapChain();

	mDevice->destroySampler(mTextureSampler);

	for (u32 i = 0; i < kMaxFramesInFlight; ++i)
	{
		mDevice->destroySemaphore(mRenderFinishedSemaphores[i]);
		mDevice->destroySemaphore(mImageAvailableSemaphores[i]);
		mDevice->destroyFence(mInFlightFences[i]);
	}
}

void IRenderer::addSwapChainListener(ISwapChainListener* listener)
{
	mSwapChainListeners.insert(listener);
}

void IRenderer::removeSwapChainListener(ISwapChainListener* listener)
{
	auto it = mSwapChainListeners.find(listener);
	if (it != mSwapChainListeners.end())
	{
		mSwapChainListeners.erase(it);
	}
}

void IRenderer::addExtCommandBufferUpdater(IExtCommandBufferUpdater* commandBufferUpdater)
{
	mCommandBufferUpdaters.insert(commandBufferUpdater);
}

void IRenderer::removeExtCommandBufferUpdater(IExtCommandBufferUpdater* commandBufferUpdater)
{
	auto it = mCommandBufferUpdaters.find(commandBufferUpdater);
	if (it != mCommandBufferUpdaters.end())
	{
		mCommandBufferUpdaters.erase(it);
	}
}

void IRenderer::recreateSwapChain()
{
	ERM_VK_CHECK(mDevice->waitIdle());

	for (auto* listener : mSwapChainListeners)
	{
		listener->swapChainCleanup();
	}

	cleanupSwapChain();

	createSwapChain();
	createFrameResources();
	createDepthResources();

	for (auto* listener : mSwapChainListeners)
	{
		listener->swapChainCreated();
	}
}

void IRenderer::cleanupSwapChain()
{
	std::for_each(mFrameBuffers.begin(), mFrameBuffers.end(), [this](auto& pair) {
		std::for_each(pair.second.begin(), pair.second.end(), [this](GPUTexture* texture) {
			(void)texture;
			(void)*this;
//			TODO: Damiano
//			mResourcesManager.releaseTexture(texture);
		});
	});

	mFrameBuffers.clear();
	mDevice->destroySwapchainKHR(mSwapChain);
}

void IRenderer::createSwapChain()
{
	const auto& frameBufferSize = mWindow.getFrameBufferSize();

	auto swapChainSupport = VkUtils::querySwapChainSupport(mDevice.getVkPhysicalDevice(), mDevice.getVkSurface());
	auto indices = VkUtils::findQueueFamilies(mDevice.getVkPhysicalDevice(), mDevice.getVkSurface());

	auto surfaceFormat = VkUtils::chooseSwapSurfaceFormat(swapChainSupport.mFormats);
	auto presentMode = VkUtils::chooseSwapPresentMode(swapChainSupport.mPresentModes);
	mSwapChainExtent = VkUtils::chooseSwapExtent(swapChainSupport.mCapabilities, frameBufferSize.x, frameBufferSize.y);
	mSwapChainImageFormat = surfaceFormat.format;

	mMinImageCount = swapChainSupport.mCapabilities.minImageCount;
	mImageCount = mMinImageCount + 1;
	if (swapChainSupport.mCapabilities.maxImageCount > 0 && mImageCount > swapChainSupport.mCapabilities.maxImageCount)
	{
		mImageCount = swapChainSupport.mCapabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.surface = mDevice.getVkSurface();
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
		u32 queueFamilyIndices[] = {indices.mGraphicsFamily.value(), indices.mPresentFamily.value()};

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

	ERM_VK_CHECK_AND_ASSIGN(mSwapChain, mDevice->createSwapchainKHR(swapChainCreateInfo));
	ERM_VK_CHECK_AND_ASSIGN(mSwapChainImages, mDevice->getSwapchainImagesKHR(mSwapChain));

	mSwapChainImageViews.resize(mSwapChainImages.size());
	mFrameBuffers[FrameBufferType::PRESENT].resize(mSwapChainImages.size());

	for (u64 i = 0; i < mSwapChainImages.size(); ++i)
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

		mSwapChainImageViews[i] = VkUtils::createImageView(
			mDevice.getVkDevice(),
			viewInfo);

		// Create texture associated
//		TODO: Damiano
//		auto* texture = mResourcesManager.createEmptyTexture();
//		texture->initWithData(
//			("present_buffer_" + std::to_string(i)).c_str(),
//			mSwapChainImages[i],
//			mSwapChainImageViews[i],
//			nullptr,
//			vk::ImageLayout::ePresentSrcKHR,
//			mSwapChainImageFormat,
//			mSwapChainExtent.width,
//			mSwapChainExtent.height);
//
//		// The present buffer images gets handled internally from the block chain, no need to delete them manually
//		texture->setOwnsImage(false);

//		mFrameBuffers[FrameBufferType::PRESENT][i] = texture;
	}
}

void IRenderer::createFrameResources()
{
	static const std::array sBuffersToCreate {FrameBufferType::FRAME_1};

	for (u64 i = 0; i < sBuffersToCreate.size(); ++i)
	{
		mFrameBuffers[sBuffersToCreate[i]].resize(IRenderer::kMaxFramesInFlight);
		for (u64 j = 0; j < IRenderer::kMaxFramesInFlight; ++j)
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

			VkUtils::createImage(
				mDevice.getVkPhysicalDevice(),
				mDevice.getVkDevice(),
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

			imageView = VkUtils::createImageView(
				mDevice.getVkDevice(),
				viewInfo);

//			TODO: Damiano
//			auto* texture = mResourcesManager.createEmptyTexture();
//			texture->initWithData(
//				("frame_buffer_1_" + std::to_string(i)).c_str(),
//				image,
//				imageView,
//				imageMemory,
//				vk::ImageLayout::eGeneral,
//				format,
//				mSwapChainExtent.width,
//				mSwapChainExtent.height);
//
//			mFrameBuffers[sBuffersToCreate[i]][j] = texture;
		}
	}
}

void IRenderer::createDepthResources()
{
	vk::Format depthFormat = VkUtils::findDepthFormat(mDevice.getVkPhysicalDevice());
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

	VkUtils::createImage(
		mDevice.getVkPhysicalDevice(),
		mDevice.getVkDevice(),
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

	depthImageView = VkUtils::createImageView(
		mDevice.getVkDevice(),
		viewInfo);

//	TODO: Damiano
//	auto* texture = mResourcesManager.createEmptyTexture();
//	texture->initWithData(
//		"depth_buffer",
//		depthImage,
//		depthImageView,
//		depthImageMemory,
//		vk::ImageLayout::eGeneral,
//		depthFormat,
//		mSwapChainExtent.width,
//		mSwapChainExtent.height);
//
//	mFrameBuffers[FrameBufferType::DEPTH].emplace_back(texture);
}

void IRenderer::createTextureSampler()
{
	// TODO: Create different samplers for different lods?
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
	samplerInfo.maxLod = 20.0f;

	ERM_VK_CHECK_AND_ASSIGN(mTextureSampler, mDevice->createSampler(samplerInfo));
}

void IRenderer::createSyncObjects()
{
	mImagesInFlight.resize(mSwapChainImages.size(), nullptr);

	vk::SemaphoreCreateInfo semaphoreInfo = {};

	vk::FenceCreateInfo fenceInfo = {};
	fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

	for (u32 i = 0; i < kMaxFramesInFlight; ++i)
	{
		ERM_VK_CHECK_AND_ASSIGN(mImageAvailableSemaphores[i], mDevice->createSemaphore(semaphoreInfo));
		ERM_VK_CHECK_AND_ASSIGN(mRenderFinishedSemaphores[i], mDevice->createSemaphore(semaphoreInfo));
		ERM_VK_CHECK_AND_ASSIGN(mInFlightFences[i], mDevice->createFence(fenceInfo));
	}
}

const std::vector<GPUTexture*>& IRenderer::getTargetFrameBuffers(FrameBufferType frameBufferType) const
{
	return mFrameBuffers.at(frameBufferType);
}

GPUTexture* IRenderer::getDefaultTexture(TextureType type) const
{
	(void)type;
//	TODO: Damiano
//	switch (type)
//	{
//		case TextureType::DIFFUSE:
//			return mResourcesManager.getOrCreateTexture("res/textures/viking_room.png");
//		case TextureType::NORMAL:
//			return mResourcesManager.getOrCreateTexture("res/textures/viking_room.png");
//		case TextureType::SPECULAR:
//			return mResourcesManager.getOrCreateTexture("res/textures/viking_room.png");
//		case TextureType::CUBE_MAP:
//			return mResourcesManager.getOrCreateCubeMap("res/cube_maps/skybox");
//		default:
//			ERM_ASSERT(false);
//			return {};
//	}
	return nullptr;
}

} // namespace erm
