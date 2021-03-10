#include "erm/rendering/renderer/IRenderer.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include "erm/rendering/window/Window.h"

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
		CreateImageViews();
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
		CreateImageViews();
		CreateDepthResources();

		for (ISwapChainListener* listener : mSwapChainListeners)
		{
			listener->SwapChainCreated();
		}
	}

	void IRenderer::CleanupSwapChain()
	{
		for (size_t i = 0; i < mSwapChainImageViews.size(); ++i)
		{
			mDevice->destroyImageView(mSwapChainImageViews[i], nullptr);
		}
		mDevice->destroyImageView(mDepthImageView);
		mDevice->destroyImage(mDepthImage);
		mDevice->freeMemory(mDepthImageMemory);
		mDevice->destroySwapchainKHR(mSwapChain, nullptr);
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
			| vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage
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
	}

	void IRenderer::CreateImageViews()
	{
		mSwapChainImageViews.resize(mSwapChainImages.size());
		for (size_t i = 0; i < mSwapChainImages.size(); ++i)
		{
			mSwapChainImageViews[i] = VkUtils::CreateImageView(
				mDevice.GetVkDevice(),
				mSwapChainImages[i],
				mSwapChainImageFormat,
				vk::ImageAspectFlagBits::eColor);
		}
	}

	void IRenderer::CreateDepthResources()
	{
		vk::Format depthFormat = VkUtils::FindDepthFormat(mDevice.GetVkPhysicalDevice());
		VkUtils::CreateImage(
			mDevice.GetVkPhysicalDevice(),
			mDevice.GetVkDevice(),
			mSwapChainExtent.width,
			mSwapChainExtent.height,
			depthFormat,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			mDepthImage,
			mDepthImageMemory);
		mDepthImageView = VkUtils::CreateImageView(mDevice.GetVkDevice(), mDepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
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
		mImageAvailableSemaphores.resize(kMaxFramesInFlight);
		mRenderFinishedSemaphores.resize(kMaxFramesInFlight);
		mInFlightFences.resize(kMaxFramesInFlight);
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

	Texture* IRenderer::GetFallbackTexture(TextureType type) const
	{
		switch (type)
		{
			case TextureType::NORMAL:
				return mResourcesManager.GetOrCreateTexture("res/textures/viking_room.png");
			case TextureType::DIFFUSE:
				return mResourcesManager.GetOrCreateTexture("res/textures/viking_room.png");
			case TextureType::SPECULAR:
				return mResourcesManager.GetOrCreateTexture("res/textures/viking_room.png");
			default:
				ASSERT(false);
		}

		return nullptr;
	}

} // namespace erm