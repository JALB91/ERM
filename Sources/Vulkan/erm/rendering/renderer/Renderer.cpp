#include "erm/rendering/renderer/Renderer.h"

#include "erm/debug/ImGuiHandle.h"

#include "erm/engine/Engine.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/ray_tracing/RTRenderingResources.h"
#endif
// clang-format on

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

Renderer::Renderer(Engine& engine)
	: IRenderer(engine)
	, mRenderingResources(nullptr)
#ifdef ERM_RAY_TRACING_ENABLED
	, mRTRenderData(nullptr)
#endif
{}

Renderer::~Renderer()
{}

void Renderer::OnPreRender()
{
	PROFILE_FUNCTION();

	if (!mRenderingResources)
		mRenderingResources = std::make_unique<RenderingResources>(mEngine);
	mRenderingResources->Refresh();
#ifdef ERM_RAY_TRACING_ENABLED
	if (!mRTRenderingResources)
		mRTRenderingResources = std::make_unique<RTRenderingResources>(mEngine);
	mRTRenderingResources->Refresh();
#endif

	VK_CHECK(mDevice->waitForFences(1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX));

	const vk::Result result = mDevice->acquireNextImageKHR(mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], {}, &mCurrentImageIndex);

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
		VK_CHECK(mDevice->waitForFences(1, &mImagesInFlight[mCurrentImageIndex], VK_TRUE, UINT64_MAX));
	}
	// Mark the image as now being in use by this frame
	mImagesInFlight[mCurrentImageIndex] = mInFlightFences[mCurrentFrame];

	mIsImageIndexValid = true;
}

void Renderer::OnRender()
{
	PROFILE_FUNCTION();

	if (!mIsImageIndexValid)
		return;

	std::vector<vk::CommandBuffer> commandBuffers = RetrieveCommandBuffers();

	vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
	vk::SubmitInfo submitInfo = {};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
	submitInfo.pCommandBuffers = commandBuffers.data();
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

	VK_CHECK(mDevice->resetFences(1, &mInFlightFences[mCurrentFrame]));
	VK_CHECK(mDevice.GetGraphicsQueue().submit(1, &submitInfo, mInFlightFences[mCurrentFrame]));
}

void Renderer::OnPostRender()
{
	PROFILE_FUNCTION();

	mRenderData.clear();
#ifdef ERM_RAY_TRACING_ENABLED
	mRTRenderData = nullptr;
#endif

	if (!mIsImageIndexValid)
		return;

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

	vk::SwapchainKHR swapChains[] = {mSwapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &mCurrentImageIndex;

	const vk::Result result = mDevice.GetPresentQueue().presentKHR(&presentInfo);

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

void Renderer::SubmitRenderData(RenderData& data)
{
	ASSERT(!data.mMeshes.empty());

	mRenderData.emplace_back(&data);
}

#ifdef ERM_RAY_TRACING_ENABLED
void Renderer::SubmitRTRenderData(RTRenderData& data)
{
	mRTRenderData = &data;
}
#endif

void Renderer::RecreateSwapChain()
{
	IRenderer::RecreateSwapChain();

	mRenderingResources.reset();
	mRenderData.clear();

#ifdef ERM_RAY_TRACING_ENABLED
	mRTRenderingResources.reset();
	mRTRenderData = nullptr;
#endif
}

std::vector<vk::CommandBuffer> Renderer::RetrieveCommandBuffers()
{
	PROFILE_FUNCTION();

	std::vector<vk::CommandBuffer> commandBuffers(
		(mRenderingResources ? 1 : 0) +
#ifdef ERM_RAY_TRACING_ENABLED
		((mRTRenderData && mRTRenderingResources) ? 1 : 0) +
#endif
		1);

	size_t index = 0;

	if (mRenderingResources)
		commandBuffers[index++] = mRenderingResources->UpdateCommandBuffer(mRenderData, mCurrentImageIndex);

#ifdef ERM_RAY_TRACING_ENABLED
	if (mRTRenderData && mRTRenderingResources)
		commandBuffers[index++] = mRTRenderingResources->UpdateCommandBuffer(*mRTRenderData, mCurrentImageIndex);
#endif

	commandBuffers[index] = mEngine.GetImGuiHandle().GetCommandBuffer(mCurrentImageIndex);

	return commandBuffers;
}

} // namespace erm
