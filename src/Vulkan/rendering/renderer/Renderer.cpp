#include "erm/rendering/renderer/Renderer.h"

#include "erm/debug/ImGuiWrapper.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/shaders/ShaderProgram.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <GLFW/glfw3.h>

#include <iostream>

namespace {

	const int kMaxFramesInFlight = 2;

}

namespace erm {

	Renderer::Renderer(GLFWwindow* window, Device& device)
		: mWindow(window)
		, mDevice(device)
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

	Renderer::~Renderer()
	{
		mDevice->waitIdle();

		for (ISwapChainListener* listener : mSwapChainListeners)
		{
			listener->SwapChainCleanup();
		}

		CleanupSwapChain();

		mDevice->destroySampler(mTextureSampler);

		for (size_t i = 0; i < kMaxFramesInFlight; ++i)
		{
			mDevice->destroySemaphore(mRenderFinishedSemaphores[i]);
			mDevice->destroySemaphore(mImageAvailableSemaphores[i]);
			mDevice->destroyFence(mInFlightFences[i]);
		}
	}

	void Renderer::OnPreRender()
	{
		if (mRenderingResources.empty())
			return;

		mDevice->waitForFences(1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

		vk::Result result = mDevice->acquireNextImageKHR(mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], {}, &mCurrentImageIndex);

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		// Check if a previous frame is using this image (i.e. there is its fence to wait on)
		if (mImagesInFlight[mCurrentImageIndex])
		{
			mDevice->waitForFences(1, &mImagesInFlight[mCurrentImageIndex], VK_TRUE, UINT64_MAX);
		}
		// Mark the image as now being in use by this frame
		mImagesInFlight[mCurrentImageIndex] = mInFlightFences[mCurrentFrame];

		mIsImageIndexValid = true;
	}

	void Renderer::OnRender()
	{
		if (!mIsImageIndexValid)
			return;

		RegisterCommandBuffers();

		std::vector<vk::CommandBuffer> commandBuffers(mRenderingResources.size());

		for (size_t i = 0; i < mRenderingResources.size(); ++i)
		{
			commandBuffers[i] = mRenderingResources[i]->mCommandBuffers[mCurrentImageIndex];
		}

		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		vk::SubmitInfo submitInfo = {};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = commandBuffers.size();
		submitInfo.pCommandBuffers = commandBuffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

		mDevice->resetFences(1, &mInFlightFences[mCurrentFrame]);

		if (mDevice.GetGraphicsQueue().submit(1, &submitInfo, mInFlightFences[mCurrentFrame]) != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to submit draw command buffer");
		}
	}

	void Renderer::OnPostRender()
	{
		mRenderData.clear();

		if (!mIsImageIndexValid)
			return;

		vk::PresentInfoKHR presentInfo = {};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

		vk::SwapchainKHR swapChains[] = {mSwapChain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &mCurrentImageIndex;

		vk::Result result = mDevice.GetPresentQueue().presentKHR(&presentInfo);

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFramebufferResized)
		{
			mFramebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to present swap chain image");
		}

		mCurrentFrame = (mCurrentFrame + 1) % kMaxFramesInFlight;

		mIsImageIndexValid = false;
	}

	void Renderer::AddSwapChainListener(ISwapChainListener* listener)
	{
		mSwapChainListeners.insert(listener);
	}

	void Renderer::RemoveSwapChainListener(ISwapChainListener* listener)
	{
		auto it = mSwapChainListeners.find(listener);
		if (it != mSwapChainListeners.end())
		{
			mSwapChainListeners.erase(it);
		}
	}

	void Renderer::SubmitRenderData(RenderData& data)
	{
		ASSERT(!data.mMehses.empty());

		RenderingResources* resources = GetOrCreateRenderingResources(data.mRenderConfigs);
		if (!resources)
			return;

		mRenderData[resources].emplace_back(&data);
	}

	RenderingResources* Renderer::GetOrCreateRenderingResources(const RenderConfigs& renderConfigs)
	{
		for (const auto& resources : mRenderingResources)
		{
			if (renderConfigs.IsRenderPassLevelCompatible(resources->mRenderConfigs) && renderConfigs.IsPipelineLevelCompatible(resources->mRenderConfigs))
			{
				return resources.get();
			}
		}

		if (!renderConfigs.mShaderProgram)
			return nullptr;

		mRenderingResources.emplace_back(std::make_unique<RenderingResources>(mDevice, *this, renderConfigs));

		return mRenderingResources.back().get();
	}

	void Renderer::RegisterCommandBuffers()
	{
		for (auto& pair : mRenderData)
		{
			pair.first->UpdateRenderingResources(pair.second, mCurrentImageIndex);
		}
	}

	void Renderer::RecreateSwapChain()
	{
		mDevice->waitIdle();

		for (ISwapChainListener* listener : mSwapChainListeners)
		{
			listener->SwapChainCleanup();
		}

		ImGuiWrapper::Cleanup();

		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
		CreateDepthResources();

		for (ISwapChainListener* listener : mSwapChainListeners)
		{
			listener->SwapChainCreated();
		}

		ImGuiWrapper::Create();
	}

	void Renderer::CleanupSwapChain()
	{
		mRenderData.clear();
		mRenderingResources.clear();
		for (size_t i = 0; i < mSwapChainImageViews.size(); ++i)
		{
			mDevice->destroyImageView(mSwapChainImageViews[i], nullptr);
		}
		mDevice->destroyImageView(mDepthImageView);
		mDevice->destroyImage(mDepthImage);
		mDevice->freeMemory(mDepthImageMemory);
		mDevice->destroySwapchainKHR(mSwapChain, nullptr);
	}

	void Renderer::CreateSwapChain()
	{
		int width, height;
		GetFrameBufferSize(width, height);

		SwapChainSupportDetails swapChainSupport = VkUtils::QuerySwapChainSupport(mDevice.GetVkPhysicalDevice(), mDevice.GetVkSurface());
		QueueFamilyIndices indices = VkUtils::FindQueueFamilies(mDevice.GetVkPhysicalDevice(), mDevice.GetVkSurface());

		vk::SurfaceFormatKHR surfaceFormat = VkUtils::ChooseSwapSurfaceFormat(swapChainSupport.mFormats);
		vk::PresentModeKHR presentMode = VkUtils::ChooseSwapPresentMode(swapChainSupport.mPresentModes);
		mSwapChainExtent = VkUtils::ChooseSwapExtent(swapChainSupport.mCapabilities, width, height);
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
		swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

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
			swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
			swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		swapChainCreateInfo.preTransform = swapChainSupport.mCapabilities.currentTransform;
		swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.clipped = VK_TRUE;
		swapChainCreateInfo.oldSwapchain = nullptr;

		mSwapChain = mDevice->createSwapchainKHR(swapChainCreateInfo);
		mSwapChainImages = mDevice->getSwapchainImagesKHR(mSwapChain);
	}

	void Renderer::CreateImageViews()
	{
		mSwapChainImageViews.resize(mSwapChainImages.size());
		for (size_t i = 0; i < mSwapChainImages.size(); ++i)
		{
			mSwapChainImageViews[i] = VkUtils::CreateImageView(mDevice.GetVkDevice(), mSwapChainImages[i], mSwapChainImageFormat, vk::ImageAspectFlagBits::eColor);
		}
	}

	void Renderer::CreateDepthResources()
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

	void Renderer::CreateTextureSampler()
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

	void Renderer::CreateSyncObjects()
	{
		mImageAvailableSemaphores.resize(kMaxFramesInFlight);
		mRenderFinishedSemaphores.resize(kMaxFramesInFlight);
		mInFlightFences.resize(kMaxFramesInFlight);
		mImagesInFlight.resize(mSwapChainImages.size(), nullptr);

		vk::SemaphoreCreateInfo semaphoreInfo = {};

		vk::FenceCreateInfo fenceInfo = {};
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		for (size_t i = 0; i < kMaxFramesInFlight; ++i)
		{
			mImageAvailableSemaphores[i] = mDevice->createSemaphore(semaphoreInfo);
			mRenderFinishedSemaphores[i] = mDevice->createSemaphore(semaphoreInfo);
			mInFlightFences[i] = mDevice->createFence(fenceInfo);
		}
	}

	void Renderer::GetFrameBufferSize(int& width, int& height)
	{
		glfwGetFramebufferSize(mWindow, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(mWindow, &width, &height);
			glfwWaitEvents();
		}
	}

} // namespace erm
