#include "erm/rendering/renderer/Renderer.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/rendering/ray_tracing/RTRenderingResources.h"
#endif
// clang-format on

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/QueueFamilyIndices.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/SwapChainSupportDetails.h"
#include "erm/rendering/renderer/IExtCommandBufferUpdater.h"
#include "erm/rendering/renderer/ISwapChainListener.h"
#include "erm/rendering/textures/GPUImage.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/log/Log.h>
#include <erm/math/Types.h>
#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/utils/Profiler.h>
#include <erm/utils/Utils.h>
#include <erm/window/Window.h>

#include <magic_enum/magic_enum.hpp>

namespace erm {

Renderer::Renderer() noexcept
: mWindow(ObjectRegistry::require<Window>())
, mDevice(ObjectRegistry::require<Device>())
, mSwapChain(nullptr)
, mTextureSampler(nullptr)
, mImageAvailableSemaphores{nullptr}
, mRenderFinishedSemaphores{nullptr}
, mInFlightFences{nullptr}
, mCurrentFrame(0)
, mCurrentImageIndex(0)
, mMinImageCount(0)
, mImageCount(0)
, mIsImageIndexValid(false)
, mFramebufferResized(true)
#ifdef ERM_RAY_TRACING_ENABLED
, mRTRenderData(nullptr)
#endif
{
	createSwapChain();
	createFrameResources();
	createDepthResources();
	createTextureSampler();
	createSyncObjects();
	createCommandBuffers();
}

Renderer::~Renderer()
{
	if (const auto result = mDevice->waitIdle(); result != vk::Result::eSuccess)
	{
		ERM_LOG_ERROR(std::format("Device idle failure: {}", magic_enum::enum_name(result)));
	}

	cleanupSwapChain();

	if (mTextureSampler)
	{
		mDevice->destroySampler(mTextureSampler);
		mTextureSampler = nullptr;
	}

	for (u32 i = 0; i < kMaxFramesInFlight; ++i)
	{
		if (mRenderFinishedSemaphores[i])
		{
			mDevice->destroySemaphore(mRenderFinishedSemaphores[i]);
			mRenderFinishedSemaphores[i] = nullptr;
		}
		
		if (mImageAvailableSemaphores[i])
		{
			mDevice->destroySemaphore(mImageAvailableSemaphores[i]);
			mImageAvailableSemaphores[i] = nullptr;
		}
		
		if (mInFlightFences[i])
		{
			mDevice->destroyFence(mInFlightFences[i]);
			mInFlightFences[i] = nullptr;
		}
	}
}

void Renderer::preRender()
{
	ERM_PROFILE_FUNCTION();

	ERM_VK_CHECK(mDevice->waitForFences(1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX));

	const vk::Result result = mDevice->acquireNextImageKHR(
		mSwapChain,
		UINT64_MAX,
		mImageAvailableSemaphores[mCurrentFrame],
		{},
		&mCurrentImageIndex);

	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		recreateSwapChain();
		return;
	}
	else if (!ERM_EXPECT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR))
	{
		ERM_LOG_ERROR("[Vulkan] vk::Device::acquireNextImageKHR failed with result: %s", magic_enum::enum_name(result).data());
		return;
	}

	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (mImagesInFlight[getCurrentImageIndex()])
	{
		ERM_VK_CHECK(mDevice->waitForFences(1, &mImagesInFlight[getCurrentImageIndex()], VK_TRUE, UINT64_MAX));
	}
	// Mark the image as now being in use by this frame
	mImagesInFlight[getCurrentImageIndex()] = mInFlightFences[mCurrentFrame];
	
	for (auto it = mRenderingMap.begin(); it != mRenderingMap.end();)
	{
		auto& [resources, data] = *it;
		
		if (resources->getUntouchedFrames() > kMaxFramesInFlight)
		{
			it = mRenderingMap.erase(it);
		}
		else
		{
			resources->refresh();
			++it;
		}
	}
#ifdef ERM_RAY_TRACING_ENABLED
	if (!mRTRenderingResources)
	{
		mRTRenderingResources = std::make_unique<RTRenderingResources>(mDevice, *this);
	}
	mRTRenderingResources->refresh();
#endif

	mIsImageIndexValid = true;
}

void Renderer::render()
{
	ERM_PROFILE_FUNCTION();

	if (!mIsImageIndexValid)
	{
		return;
	}

	vk::CommandBuffer& commandBuffer = retrieveCommandBuffer();

	vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
	vk::SubmitInfo submitInfo = {};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

	ERM_VK_CHECK(mDevice->resetFences(1, &mInFlightFences[mCurrentFrame]));
	ERM_VK_CHECK(mDevice.getGraphicsQueue().submit(1, &submitInfo, mInFlightFences[mCurrentFrame]));
}

void Renderer::postRender()
{
	ERM_PROFILE_FUNCTION();

	for (auto& [resources, data] : mRenderingMap)
	{
		data.clear();
	}

#ifdef ERM_RAY_TRACING_ENABLED
	mRTRenderData = nullptr;
#endif

	if (!mIsImageIndexValid)
	{
		return;
	}

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

	vk::SwapchainKHR swapChains[] = {mSwapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &mCurrentImageIndex;

	const vk::Result result = mDevice.getPresentQueue().presentKHR(&presentInfo);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFramebufferResized)
	{
		mFramebufferResized = false;
		recreateSwapChain();
	}
	else if (!ERM_EXPECT(result == vk::Result::eSuccess))
	{
		ERM_LOG_ERROR("[Vulkan] vk::Queue::presentKHR failed with result: %s", magic_enum::enum_name(result).data());
		return;
	}

	mCurrentFrame = (mCurrentFrame + 1) % kMaxFramesInFlight;

	mIsImageIndexValid = false;
}

void Renderer::addSwapChainListener(ISwapChainListener* listener)
{
	mSwapChainListeners.insert(listener);
}

void Renderer::removeSwapChainListener(ISwapChainListener* listener)
{
	auto it = mSwapChainListeners.find(listener);
	if (it != mSwapChainListeners.end())
	{
		mSwapChainListeners.erase(it);
	}
}

void Renderer::addExtCommandBufferUpdater(IExtCommandBufferUpdater* commandBufferUpdater)
{
	mCommandBufferUpdaters.insert(commandBufferUpdater);
}

void Renderer::removeExtCommandBufferUpdater(IExtCommandBufferUpdater* commandBufferUpdater)
{
	auto it = mCommandBufferUpdaters.find(commandBufferUpdater);
	if (it != mCommandBufferUpdaters.end())
	{
		mCommandBufferUpdaters.erase(it);
	}
}

void Renderer::recreateSwapChain()
{
	ERM_VK_CHECK(mDevice->waitIdle());

	cleanupSwapChain();

	createSwapChain();
	createFrameResources();
	createDepthResources();
	
	mRenderingMap.clear();

#ifdef ERM_RAY_TRACING_ENABLED
	mRTRenderingResources.reset();
	mRTRenderData = nullptr;
#endif

	mCommandBuffers.clear();

	createCommandBuffers();

	for (auto* listener : mSwapChainListeners)
	{
		listener->swapChainCreated();
	}
}

void Renderer::cleanupSwapChain()
{
	for (auto* listener : mSwapChainListeners)
	{
		listener->swapChainCleanup();
	}

	mFrameBuffers.clear();
	
	if (mSwapChain)
	{
		mDevice->destroySwapchainKHR(mSwapChain);
		mSwapChain = nullptr;
	}
}

void Renderer::createSwapChain()
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
		auto texture = std::make_unique<GPUTexture>(
			mDevice,
			Texture {
				.mBuffer = nullptr,
				.mWidth = mSwapChainExtent.width,
				.mHeight = mSwapChainExtent.height,
				.mBPP = 4
			},
			0,
			0,
			mSwapChainImages[i],
			mSwapChainImageViews[i],
			nullptr,
			vk::ImageLayout::ePresentSrcKHR,
			mSwapChainImageFormat,
			false
		);

		mFrameBuffers[FrameBufferType::PRESENT][i] = std::move(texture);
	}
}

void Renderer::createFrameResources()
{
	static constexpr std::array sBuffersToCreate {FrameBufferType::FRAME_1};

	for (u64 i = 0; i < sBuffersToCreate.size(); ++i)
	{
		mFrameBuffers[sBuffersToCreate[i]].resize(Renderer::kMaxFramesInFlight);
		for (u64 j = 0; j < Renderer::kMaxFramesInFlight; ++j)
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

			// Create texture associated
			auto texture = std::make_unique<GPUTexture>(
				mDevice,
				Texture {
					.mBuffer = nullptr,
					.mWidth = mSwapChainExtent.width,
					.mHeight = mSwapChainExtent.height,
					.mBPP = 4},
				0,
				0,
				image,
				imageView,
				imageMemory,
				vk::ImageLayout::eGeneral,
				format,
				true
			);

			mFrameBuffers[sBuffersToCreate[i]][j] = std::move(texture);
		}
	}
}

void Renderer::createDepthResources()
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

	auto texture = std::make_unique<GPUTexture>(
		mDevice,
		Texture {
			.mBuffer = nullptr,
			.mWidth = mSwapChainExtent.width,
			.mHeight = mSwapChainExtent.height,
			.mBPP = 4},
		0,
		0,
		depthImage,
		depthImageView,
		depthImageMemory,
		vk::ImageLayout::eGeneral,
		depthFormat,
		true
	);

	mFrameBuffers[FrameBufferType::DEPTH].emplace_back(std::move(texture));
}

void Renderer::createTextureSampler()
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

void Renderer::createSyncObjects()
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

const std::vector<std::unique_ptr<GPUTexture>>& Renderer::getTargetFrameBuffers(FrameBufferType frameBufferType) const
{
	return mFrameBuffers.at(frameBufferType);
}

GPUTexture* Renderer::getDefaultTexture(TextureType type) const
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

void Renderer::submitRenderData(RenderData& data)
{
	ERM_ASSERT(!data.mMeshes.empty());

	const auto it = std::find_if(mRenderingMap.begin(), mRenderingMap.end(), [&data](const auto& resources) {
		return resources.first->getRenderConfigs().isRenderPassLevelCompatible(data.mRenderConfigs);
	});

	if (it == mRenderingMap.end())
	{
		mRenderingMap[std::make_unique<RenderingResources>(mDevice, mWindow, *this, data.mRenderConfigs)] = {&data};
	}
	else
	{
		it->second.emplace_back(&data);
	}
}

#ifdef ERM_RAY_TRACING_ENABLED
void Renderer::submitRTRenderData(RTRenderData& data)
{
	mRTRenderData = &data;
}
#endif

void Renderer::createCommandBuffers()
{
	mCommandBuffers.resize(kMaxFramesInFlight);

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = mDevice.getCommandPool();
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = static_cast<u32>(mCommandBuffers.size());

	ERM_VK_CHECK_AND_ASSIGN(mCommandBuffers, mDevice->allocateCommandBuffersUnique(allocInfo));
}

vk::CommandBuffer& Renderer::retrieveCommandBuffer()
{
	ERM_PROFILE_FUNCTION();

	vk::CommandBuffer& cmd = mCommandBuffers[getCurrentFrame()].get();
	cmd.reset({});

	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = {};
	beginInfo.pInheritanceInfo = nullptr;

	ERM_VK_CHECK(cmd.begin(beginInfo));

	for (auto& [resources, data] : mRenderingMap)
	{
		if (!data.empty())
		{
			resources->updateCommandBuffer(cmd, data);
		}
	}

#ifdef ERM_RAY_TRACING_ENABLED
	if (mRTRenderData && mRTRenderingResources)
	{
		mRTRenderingResources->updateCommandBuffer(cmd, *mRTRenderData);
	}
#endif

	for (auto* updater : mCommandBufferUpdaters)
	{
		updater->updateCommandBuffer(cmd);
	}

	ERM_VK_CHECK(cmd.end());

	return cmd;
}

} // namespace erm
