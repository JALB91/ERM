#pragma once

#include "erm/rendering/data_structs/IBindingResources.h"
#include "erm/rendering/data_structs/PipelineData.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <deque>
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

		void UpdateResources(vk::CommandBuffer& cmd, RenderData& renderData, uint32_t imageIndex);
		void UpdateCommandBuffer(vk::CommandBuffer& cmd, RenderData& renderData, uint32_t imageIndex);

		inline const RenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }

	private:
		void CreatePipeline();
		PipelineData& GetOrCreatePipelineData(RenderData& renderData);

		Device& mDevice;
		Renderer& mRenderer;
		const vk::RenderPass* mRenderPass;
		const vk::DescriptorPool* mDescriptorPool;

		const RenderConfigs mRenderConfigs;
		vk::UniquePipelineLayout mPipelineLayout;
		vk::UniquePipeline mPipeline;

		vk::DescriptorSetLayout mEmptySetLayout;
		vk::DescriptorSet mEmptySet;
		std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
		std::deque<PipelineData> mData;
	};

} // namespace erm
