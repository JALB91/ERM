#pragma once

#include "erm/ray_tracing/RTTlas.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {
class Engine;
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
	RTRenderingResources(Engine& engine);
	~RTRenderingResources();

	vk::CommandBuffer UpdateCommandBuffer(RTRenderData& renderData, uint32_t imageIndex);

	void Refresh();

private:
	void UpdateResources(RTRenderData& renderData, uint32_t imageIndex);
	void BuildBlas(RTRenderData& data, vk::BuildAccelerationStructureFlagsKHR flags);
	void UpdateTopLevelAS(RTRenderData& data, vk::BuildAccelerationStructureFlagsKHR flags);

	void Reload();
	void Cleanup();

	void CreateDescriptorPool();
	void CreateCommandBuffers();

	Device& mDevice;
	IRenderer& mRenderer;

	vk::UniqueDescriptorPool mDescriptorPool;
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;

	RTTlas mTopLevelAS;
	std::unique_ptr<RTPipelineResources> mPipelineResources;
};

} // namespace erm
