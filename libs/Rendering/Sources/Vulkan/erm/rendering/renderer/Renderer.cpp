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

#include <erm/window/Window.h>

#include <erm/utils/Profiler.h>
#include <erm/utils/Utils.h>

namespace erm {

Renderer::Renderer(Window& window, Device& device)
	: IRenderer(window, device)
#ifdef ERM_RAY_TRACING_ENABLED
	, mRTRenderData(nullptr)
#endif
{
	CreateCommandBuffers();
}

Renderer::~Renderer()
{}

void Renderer::OnPreRender()
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
		RecreateSwapChain();
		return;
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		ERM_ASSERT_HARD(false);
	}

	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (mImagesInFlight[GetCurrentImageIndex()])
	{
		ERM_VK_CHECK(mDevice->waitForFences(1, &mImagesInFlight[GetCurrentImageIndex()], VK_TRUE, UINT64_MAX));
	}
	// Mark the image as now being in use by this frame
	mImagesInFlight[GetCurrentImageIndex()] = mInFlightFences[mCurrentFrame];
	
	for (auto it = mRenderingMap.begin(); it != mRenderingMap.end();)
	{
		auto& [resources, data] = *it;
		
		if (resources->GetUntouchedFrames() > kMaxFramesInFlight)
		{
			it = mRenderingMap.erase(it);
		}
		else
		{
			resources->Refresh();
			++it;
		}
	}
#ifdef ERM_RAY_TRACING_ENABLED
	if (!mRTRenderingResources)
		mRTRenderingResources = std::make_unique<RTRenderingResources>(mDevice, *this);
	mRTRenderingResources->Refresh();
#endif

	mIsImageIndexValid = true;
}

void Renderer::OnRender()
{
	ERM_PROFILE_FUNCTION();

	if (!mIsImageIndexValid)
		return;

	vk::CommandBuffer& commandBuffer = RetrieveCommandBuffer();

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
	ERM_VK_CHECK(mDevice.GetGraphicsQueue().submit(1, &submitInfo, mInFlightFences[mCurrentFrame]));
}

void Renderer::OnPostRender()
{
	ERM_PROFILE_FUNCTION();

	for (auto& [resources, data] : mRenderingMap)
		data.clear();

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
		ERM_ASSERT_HARD(false);
	}

	mCurrentFrame = (mCurrentFrame + 1) % kMaxFramesInFlight;

	mIsImageIndexValid = false;
}

void Renderer::SubmitRenderData(RenderData& data)
{
	ERM_ASSERT(!data.mMeshes.empty());

	const auto it = std::find_if(mRenderingMap.begin(), mRenderingMap.end(), [&data](const auto& resources) {
		return resources.first->GetRenderConfigs().IsRenderPassLevelCompatible(data.mRenderConfigs);
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
void Renderer::SubmitRTRenderData(RTRenderData& data)
{
	mRTRenderData = &data;
}
#endif

void Renderer::RecreateSwapChain()
{
	IRenderer::RecreateSwapChain();

	mRenderingMap.clear();

#ifdef ERM_RAY_TRACING_ENABLED
	mRTRenderingResources.reset();
	mRTRenderData = nullptr;
#endif

	mCommandBuffers.clear();

	CreateCommandBuffers();
}

void Renderer::CreateCommandBuffers()
{
	mCommandBuffers.resize(IRenderer::kMaxFramesInFlight);

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = mDevice.GetCommandPool();
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

	ERM_VK_CHECK_AND_ASSIGN(mCommandBuffers, mDevice->allocateCommandBuffersUnique(allocInfo));
}

vk::CommandBuffer& Renderer::RetrieveCommandBuffer()
{
	ERM_PROFILE_FUNCTION();

	vk::CommandBuffer& cmd = mCommandBuffers[GetCurrentFrame()].get();
	cmd.reset({});

	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = {};
	beginInfo.pInheritanceInfo = nullptr;

	ERM_VK_CHECK(cmd.begin(beginInfo));

	for (auto& [resources, data] : mRenderingMap)
	{
		if (!data.empty())
			resources->UpdateCommandBuffer(cmd, data);
	}

#ifdef ERM_RAY_TRACING_ENABLED
	if (mRTRenderData && mRTRenderingResources)
		mRTRenderingResources->UpdateCommandBuffer(cmd, *mRTRenderData);
#endif

	for (IExtCommandBufferUpdater* updater : mCommandBufferUpdaters)
		updater->UpdateCommandBuffer(cmd);

	ERM_VK_CHECK(cmd.end());

	return cmd;
}

} // namespace erm
