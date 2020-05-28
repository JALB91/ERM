#include "erm/rendering/renderer/Renderer.h"

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
		, mIsImageIndexValid(false)
		, mFramebufferResized(true)
	{
		CreateSwapChain();
		CreateImageViews();
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

		for (const auto& resources : mRenderingResources)
		{
			const std::vector<vk::CommandBuffer>& cmdBuffers = resources->GetCommandBuffers();

			vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
			vk::SubmitInfo submitInfo = {};
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame];
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &cmdBuffers[GetCurrentImageIndex()];
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

			mDevice->resetFences(1, &mInFlightFences[mCurrentFrame]);

			if (mDevice.GetGraphicsQueue().submit(1, &submitInfo, mInFlightFences[mCurrentFrame]) != vk::Result::eSuccess)
			{
				throw std::runtime_error("Failed to submit draw command buffer");
			}
		}

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
	}

	void Renderer::OnPostRender()
	{
		mIsImageIndexValid = false;
		for (const auto& resources : mRenderingResources)
		{
			for (const vk::CommandBuffer& buffer : resources->GetCommandBuffers())
			{
				buffer.reset({});
			}
		}
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

	size_t Renderer::GetCurrentFrame() const
	{
		return mCurrentFrame;
	}

	uint32_t Renderer::GetCurrentImageIndex() const
	{
		return mCurrentImageIndex;
	}

	vk::Extent2D Renderer::GetSwapChainExtent() const
	{
		return mSwapChainExtent;
	}

	vk::Sampler Renderer::GetTextureSampler() const
	{
		return mTextureSampler;
	}

	void Renderer::SubmitRenderData(const RenderData& data)
	{
		ASSERT(!data.mMehses.empty());

		const RenderingResources* resources = GetOrCreateRenderingResources(data.mRenderConfigs);
		if (!resources)
			return;

		resources->UpdateUniformBuffer(GetCurrentImageIndex(), data.mUniformBuffer);

		const std::vector<vk::CommandBuffer>& commandBuffers = resources->GetCommandBuffers();

		for (size_t i = 0; i < commandBuffers.size(); ++i)
		{
			const vk::CommandBuffer& commandBuffer = commandBuffers[i];

			vk::CommandBufferBeginInfo beginInfo = {};
			beginInfo.flags = {}; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			commandBuffer.begin(beginInfo);

			std::array<vk::ClearValue, 2> clearValues {};
			clearValues[0].color.float32[0] = 0.0f;
			clearValues[0].color.float32[1] = 0.0f;
			clearValues[0].color.float32[2] = 0.0f;
			clearValues[0].color.float32[3] = 1.0f;
			clearValues[1].setDepthStencil({1.0f, 0});

			vk::RenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.renderPass = resources->mRenderPass;
			renderPassInfo.framebuffer = resources->mSwapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = vk::Offset2D {0, 0};
			renderPassInfo.renderArea.extent = mSwapChainExtent;
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, resources->mPipeline);

			for (const Mesh* mesh : data.mMehses)
			{
				mesh->GetVertexBuffer().Bind(commandBuffer);
				mesh->GetIndexBuffer().Bind(commandBuffer);
				commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, resources->mPipelineLayout, 0, 1, &resources->mDescriptorSets[i], 0, nullptr);
				commandBuffer.drawIndexed(mesh->GetIndexBuffer().GetCount(), 1, 0, 0, 0);
			}

			commandBuffer.endRenderPass();
			commandBuffer.end();
		}
	}

	const RenderingResources* Renderer::GetOrCreateRenderingResources(const RenderConfigs& renderConfigs)
	{
		for (const auto& resources : mRenderingResources)
		{
			if (resources->GetRenderConfigs() == renderConfigs)
			{
				return resources.get();
			}
		}

		if (!renderConfigs.mShaderProgram)
			return nullptr;

		mRenderingResources.emplace_back(std::make_unique<RenderingResources>(mDevice, renderConfigs, mSwapChainImageViews, mSwapChainImageFormat, mTextureSampler));

		return mRenderingResources.back().get();
	}

	void Renderer::RecreateSwapChain()
	{
		mDevice->waitIdle();

		for (ISwapChainListener* listener : mSwapChainListeners)
		{
			listener->SwapChainCleanup();
		}

		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();

		for (ISwapChainListener* listener : mSwapChainListeners)
		{
			listener->SwapChainCreated();
		}
	}

	void Renderer::CleanupSwapChain()
	{
		mRenderingResources.clear();
		for (size_t i = 0; i < mSwapChainImageViews.size(); ++i)
		{
			mDevice->destroyImageView(mSwapChainImageViews[i], nullptr);
		}
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

		uint32_t imageCount = swapChainSupport.mCapabilities.minImageCount + 1;
		if (swapChainSupport.mCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.mCapabilities.maxImageCount)
		{
			imageCount = swapChainSupport.mCapabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.surface = mDevice.GetVkSurface();
		swapChainCreateInfo.minImageCount = imageCount;
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
