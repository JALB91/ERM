#include "erm/rendering/renderer/Renderer.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/rendering/ray_tracing/RTRenderingResources.h"
#endif
// clang-format on

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/IExtCommandBufferUpdater.h"
#include "erm/rendering/textures/GPUImage.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/log/Log.h>

#include <erm/math/Types.h>

#include <erm/window/Window.h>

#include <erm/utils/Profiler.h>
#include <erm/utils/Utils.h>

#include <magic_enum/magic_enum.hpp>

namespace erm {

Renderer::Renderer(Window& window, Device& device)
	: IRenderer(window, device)
#ifdef ERM_RAY_TRACING_ENABLED
	, mRTRenderData(nullptr)
#endif
{
	createCommandBuffers();
}

Renderer::~Renderer()
{}

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

void Renderer::recreateSwapChain()
{
	IRenderer::recreateSwapChain();

	mRenderingMap.clear();

#ifdef ERM_RAY_TRACING_ENABLED
	mRTRenderingResources.reset();
	mRTRenderData = nullptr;
#endif

	mCommandBuffers.clear();

	createCommandBuffers();
}

void Renderer::createCommandBuffers()
{
	mCommandBuffers.resize(IRenderer::kMaxFramesInFlight);

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
