#pragma once

#include "erm/rendering/data_structs/BindingResources.h"
#include "erm/rendering/data_structs/PipelineResources.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

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
		~RenderingResources();

		RenderingResources(RenderingResources&& other);

		RenderingResources(const RenderingResources&) = delete;

		RenderingResources& operator=(const RenderingResources&) = delete;
		RenderingResources& operator=(RenderingResources&&) = delete;

		void AddSubpass(const SubpassData& data);
		bool IsSubpassCompatible(const SubpassData& subpass) const;
		void Update(std::vector<RenderData*>& renderData, uint32_t imageIndex);

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

	public:
		vk::RenderPass mRenderPass;
		std::vector<vk::Framebuffer> mSwapChainFramebuffers;
		vk::DescriptorPool mDescriptorPool;
		std::vector<PipelineResources> mPipelineResources;
		std::vector<vk::CommandBuffer> mCommandBuffers;
	};

} // namespace erm
