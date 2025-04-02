#pragma once

#include "erm/rendering/ray_tracing/RTTlas.h"

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

	void refresh();
	void updateCommandBuffer(vk::CommandBuffer& cmd, RTRenderData& renderData);

private:
	void updateResources(RTRenderData& renderData);
	void buildBlas(RTRenderData& data, vk::BuildAccelerationStructureFlagsKHR flags);
	void updateTopLevelAS(RTRenderData& data, vk::BuildAccelerationStructureFlagsKHR flags);

	void reload();
	void cleanup();

	void createDescriptorPool();

	Device& mDevice;
	IRenderer& mRenderer;

	vk::UniqueDescriptorPool mDescriptorPool;

	RTTlas mTopLevelAS;
	std::unique_ptr<RTPipelineResources> mPipelineResources;
};

} // namespace erm
