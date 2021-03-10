#pragma once

#include "erm/rendering/data_structs/PipelineResources.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {
	class Device;
	class Renderer;
	struct RenderData;
} // namespace erm

namespace erm {

	class RenderingResources
	{
	public:
		RenderingResources(
			Device& device,
			Renderer& renderer,
			const std::vector<SubpassData>& renderConfigs);
		~RenderingResources() = default;

		RenderingResources(RenderingResources&& other);

		RenderingResources(const RenderingResources&) = delete;

		RenderingResources& operator=(const RenderingResources&) = delete;
		RenderingResources& operator=(RenderingResources&&) = delete;

		vk::CommandBuffer UpdateCommandBuffer(std::vector<RenderData*>& renderData, uint32_t imageIndex);

		void AddSubpass(const SubpassData& data);
		bool IsSubpassCompatible(const SubpassData& subpass) const;
		void Refresh();

	private:
		vk::AttachmentDescription CreateAttachmentDescription(const erm::AttachmentData& data, vk::Format format) const;
		PipelineResources& GetOrCreatePipelineResources(const RenderConfigs& renderConfigs);

		void Reload();
		void Cleanup();

		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateDescriptorPool();
		void CreateCommandBuffers();

		Device& mDevice;
		Renderer& mRenderer;
		std::vector<SubpassData> mSubpassData;
		vk::UniqueRenderPass mRenderPass;
		std::vector<vk::UniqueFramebuffer> mSwapChainFramebuffers;
		vk::UniqueDescriptorPool mDescriptorPool;
		std::vector<std::unique_ptr<PipelineResources>> mPipelineResources;
		std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
	};

} // namespace erm
