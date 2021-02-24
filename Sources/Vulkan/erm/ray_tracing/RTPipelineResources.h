#pragma once

#include "erm/rendering/buffers/HostBuffer.h"
#include "erm/rendering/data_structs/PipelineData.h"

#include "erm/ray_tracing/RTRenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <deque>

namespace erm {
	class Device;
	class IRenderer;
	struct RTRenderData;
} // namespace erm

namespace erm {

	class RTPipelineResources
	{
	public:
		RTPipelineResources(
			Device& device,
			IRenderer& renderer,
			const RTRenderConfigs& renderConfigs,
			const vk::DescriptorPool& descriptorPool,
			const vk::AccelerationStructureKHR* topLevelAS);

		void UpdateResources(vk::CommandBuffer& cmd, RTRenderData& renderData, uint32_t imageIndex);
		void UpdateCommandBuffer(vk::CommandBuffer& cmd, RTRenderData& renderData, uint32_t imageIndex);

		inline const RTRenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }
		inline const vk::Buffer GetSBTBuffer() const { return mSBTBuffer->GetBuffer(); }

	private:
		void CreatePipeline();
		void CreateBindingTable();
		void CreatePipelineData();

		Device& mDevice;
		IRenderer& mRenderer;
		RTRenderConfigs mRenderConfigs;
		const vk::DescriptorPool& mDescriptorPool;
		const vk::AccelerationStructureKHR* mTopLevelAS;

		vk::UniquePipelineLayout mPipelineLayout;
		vk::UniquePipeline mPipeline;

		vk::UniqueDescriptorSetLayout mEmptySetLayout;
		vk::UniqueDescriptorSet mEmptySet;
		std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;

		std::unique_ptr<HostBuffer> mSBTBuffer;
		std::unique_ptr<PipelineData> mPipelineData;
	};

} // namespace erm