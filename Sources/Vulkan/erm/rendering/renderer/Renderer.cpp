#include "erm/rendering/renderer/Renderer.h"

#include "erm/debug/ImGuiHandle.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/ray_tracing/RTBlas.h"
#include "erm/ray_tracing/RTRenderData.h"
#include "erm/ray_tracing/RTRenderingResources.h"
#endif
// clang-format on

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <iostream>

namespace erm {

	Renderer::Renderer(Engine& engine)
		: IRenderer(engine)
	{}

	Renderer::~Renderer()
	{}

	void Renderer::OnPreRender()
	{
		PROFILE_FUNCTION();

#ifdef ERM_RAY_TRACING_ENABLED
		for (auto it = mRTRenderData.begin(); it != mRTRenderData.end();)
		{
			if (it->second == nullptr)
				it = mRTRenderData.erase(it);
			else
				++it;
		}
#endif

		if (mRasterData.empty()
#ifdef ERM_RAY_TRACING_ENABLED
			&& mRTRenderData.empty()
#endif
		)
			return;

		for (const auto& [renderingResources, renderData] : mRasterData)
		{
			renderingResources->Refresh();
		}
#ifdef ERM_RAY_TRACING_ENABLED
		for (const auto& [resources, data] : mRTRenderData)
			resources->Refresh();
#endif

		VK_CHECK(mDevice->waitForFences(1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX));

		const vk::Result result = mDevice->acquireNextImageKHR(mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], {}, &mCurrentImageIndex);

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			RecreateSwapChain();
			mRasterData.clear();
#ifdef ERM_RAY_TRACING_ENABLED
			mRTRenderData.clear();
#endif
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

		for (RasterData::value_type& data : mRasterData)
		{
			data.second.clear();
		}

#ifdef ERM_RAY_TRACING_ENABLED
		for (auto& [resources, data] : mRTRenderData)
			data = nullptr;
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
			mRasterData.clear();
#ifdef ERM_RAY_TRACING_ENABLED
			mRTRenderData.clear();
#endif
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

		RasterData::value_type& framesData = GetOrCreateFramesData(data.mRenderConfigs);

		framesData.second.emplace_back(&data);
	}

#ifdef ERM_RAY_TRACING_ENABLED
	void Renderer::SubmitRTRenderData(RTRenderData& data)
	{
		for (auto& [resources, rtData] : mRTRenderData)
		{
			if (resources->GetRenderConfigs().mShaderProgram == data.mRenderConfigs.mShaderProgram)
			{
				ASSERT(rtData == nullptr);
				rtData = &data;
				return;
			}
		}

		mRTRenderData.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(std::make_unique<RTRenderingResources>(mDevice, *this, data.mRenderConfigs)),
			std::forward_as_tuple(&data));
	}
#endif

	std::vector<vk::CommandBuffer> Renderer::RetrieveCommandBuffers()
	{
		PROFILE_FUNCTION();

		std::vector<vk::CommandBuffer> commandBuffers(mRasterData.size() +
#ifdef ERM_RAY_TRACING_ENABLED
													  mRTRenderData.size() +
#endif
													  1);

		size_t index = 0;

		for (RasterData::value_type& data : mRasterData)
		{
			commandBuffers[index++] = data.first->UpdateCommandBuffer(data.second, mCurrentImageIndex);
		}

#ifdef ERM_RAY_TRACING_ENABLED
		for (auto& [resources, data] : mRTRenderData)
		{
			ASSERT(data);
			resources->Update(*data, mCurrentImageIndex);
			commandBuffers[index++] = resources->UpdateCommandBuffer(*data, mCurrentImageIndex);
		}
#endif

		commandBuffers[index] = mEngine.GetImGuiHandle().GetCommandBuffer(mCurrentImageIndex);

		return commandBuffers;
	}

	Renderer::RasterData::value_type& Renderer::GetOrCreateFramesData(const RenderConfigs& renderConfigs)
	{
		for (RasterData::value_type& data : mRasterData)
		{
			if (data.first->IsSubpassCompatible(renderConfigs.mSubpassData))
				return data;
		}

		std::vector<SubpassData> data = {renderConfigs.mSubpassData};

		auto it = mRasterData.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(std::make_unique<RenderingResources>(mDevice, *this, std::move(data))),
			std::forward_as_tuple());

		return *it.first;
	}

} // namespace erm
