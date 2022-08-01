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

	for (auto& [key, value] : mRenderingMap)
		value.first->Refresh();
#ifdef ERM_RAY_TRACING_ENABLED
	if (!mRTRenderingResources)
		mRTRenderingResources = std::make_unique<RTRenderingResources>(mEngine);
	mRTRenderingResources->Refresh();
#endif

	ERM_VK_CHECK(mDevice->waitForFences(1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX));

	const vk::Result result = mDevice->acquireNextImageKHR(mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], {}, &mCurrentImageIndex);

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

	for (auto& [key, value] : mRenderingMap)
		value.second.clear();

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

	if (data.mRenderingId.has_value())
	{
		const auto it = mRenderingMap.find(data.mRenderingId.value());

		if (it == mRenderingMap.end())
		{
			data.mRenderingId = static_cast<uint32_t>(mRenderingMap.size());
			mRenderingMap[data.mRenderingId.value()] = std::make_pair<std::unique_ptr<RenderingResources>, std::vector<RenderData*>>(std::make_unique<RenderingResources>(mEngine, data.mRenderConfigs), {&data});
		}
		else
		{
			it->second.second.emplace_back(&data);
		}
	}
	else
	{
		const auto it = std::find_if(mRenderingMap.begin(), mRenderingMap.end(), [&data](const auto& resources) {
			return resources.second.first->GetRenderConfigs().IsRenderPassLevelCompatible(data.mRenderConfigs);
		});

		if (it == mRenderingMap.end())
		{
			data.mRenderingId = static_cast<uint32_t>(mRenderingMap.size());
			mRenderingMap[data.mRenderingId.value()] = std::make_pair<std::unique_ptr<RenderingResources>, std::vector<RenderData*>>(std::make_unique<RenderingResources>(mEngine, data.mRenderConfigs), {&data});
		}
		else
		{
			data.mRenderingId = it->first;
			it->second.second.emplace_back(&data);
		}
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

	for (auto& [key, value] : mRenderingMap)
		if (!value.second.empty())
			value.first->UpdateCommandBuffer(cmd, value.second);

#ifdef ERM_RAY_TRACING_ENABLED
	if (mRTRenderData && mRTRenderingResources)
		mRTRenderingResources->UpdateCommandBuffer(cmd, *mRTRenderData);
#endif

	mEngine.GetImGuiHandle().UpdateCommandBuffer(cmd);

	ERM_VK_CHECK(cmd.end());

	return cmd;
}

} // namespace erm
