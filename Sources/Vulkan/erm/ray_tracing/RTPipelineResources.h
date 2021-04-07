#pragma once

#include "erm/rendering/buffers/HostBuffer.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
class Device;
class IRenderer;
class PipelineData;
struct RTRenderData;
} // namespace erm

namespace erm {

class RTPipelineResources
{
public:
	RTPipelineResources(
		Device& device,
		IRenderer& renderer,
		const RTRenderData& renderData,
		const vk::DescriptorPool& descriptorPool,
		const vk::AccelerationStructureKHR* topLevelAS);
	~RTPipelineResources();

	void Refresh();

	void UpdateCommandBuffer(vk::CommandBuffer& cmd, RTRenderData& renderData, uint32_t imageIndex);

	inline const vk::Buffer GetSBTBuffer() const { return mSBTBuffer->GetBuffer(); }
	inline size_t GetMaxInstancesCount() const { return mMaxInstancesCount; }

private:
	void CreatePipeline();
	void CreateBindingTable();
	void CreatePipelineData();

	Device& mDevice;
	IRenderer& mRenderer;
	const RTRenderData& mRenderData;
	const vk::DescriptorPool& mDescriptorPool;
	const vk::AccelerationStructureKHR* mTopLevelAS;

	vk::UniquePipelineLayout mPipelineLayout;
	vk::UniquePipeline mPipeline;

	vk::UniqueDescriptorSetLayout mEmptySetLayout;
	vk::UniqueDescriptorSet mEmptySet;
	std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;

	std::unique_ptr<HostBuffer> mSBTBuffer;
	std::unique_ptr<PipelineData> mPipelineData;
	size_t mMaxInstancesCount;
};

} // namespace erm
