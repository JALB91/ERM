#pragma once

#include "erm/ray_tracing/RTTlas.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {
class Device;
class IRenderer;
class RTBlas;
class RTPipelineResources;
struct RTRenderData;
} // namespace erm

namespace erm {

class RTRenderingResources
{
public:
	RTRenderingResources(Device& device, IRenderer& renderer);
	~RTRenderingResources();

	void Refresh();
	void UpdateCommandBuffer(vk::CommandBuffer& cmd, RTRenderData& renderData);

private:
	void UpdateResources(RTRenderData& renderData);
	void BuildBlas(RTRenderData& data, vk::BuildAccelerationStructureFlagsKHR flags);
	void UpdateTopLevelAS(RTRenderData& data, vk::BuildAccelerationStructureFlagsKHR flags);

	void Reload();
	void Cleanup();

	void CreateDescriptorPool();

	Device& mDevice;
	IRenderer& mRenderer;

	vk::UniqueDescriptorPool mDescriptorPool;

	RTTlas mTopLevelAS;
	std::unique_ptr<RTPipelineResources> mPipelineResources;
};

} // namespace erm
