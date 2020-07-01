#pragma once

#include "erm/rendering/data_structs/BindingResources.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
	class Device;
	class Renderer;
	struct RenderData;
} // namespace erm

namespace erm {

	class PipelineResources
	{
	public:
		PipelineResources(
			Device& device,
			Renderer& renderer,
			const vk::RenderPass* renderPass,
			const vk::DescriptorPool* descriptorPool,
			const RenderConfigs& renderConfigs);
		~PipelineResources();

		PipelineResources(PipelineResources&&) = delete;
		PipelineResources& operator=(PipelineResources&&) = delete;

		PipelineResources(const PipelineResources&) = delete;
		PipelineResources& operator=(const PipelineResources&) = delete;

		void Update(vk::CommandBuffer& cmd, RenderData& renderData, uint32_t imageIndex);

	private:
		void CreatePipeline();

		Device& mDevice;
		Renderer& mRenderer;
		const vk::RenderPass* mRenderPass;
		const vk::DescriptorPool* mDescriptorPool;

	public:
		const RenderConfigs mRenderConfigs;
		vk::PipelineLayout mPipelineLayout;
		vk::UniquePipeline mPipeline;

		vk::DescriptorSetLayout mDescriptorSetLayout;

		std::vector<BindingResources> mBindingResources;
	};

} // namespace erm
