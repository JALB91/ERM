#pragma once

#include "erm/rendering/buffers/HostBuffer.h"

#include <erm/math/Types.h>

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

	void refresh();

	void updateCommandBuffer(vk::CommandBuffer& cmd, RTRenderData& renderData);

	inline const vk::Buffer getSBTBuffer() const { return mSBTBuffer->getBuffer(); }
	inline u64 getMaxInstancesCount() const { return mMaxInstancesCount; }

private:
	void createPipeline();
	void createBindingTable();
	void createPipelineData();

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
	u64 mMaxInstancesCount;
};

} // namespace erm
