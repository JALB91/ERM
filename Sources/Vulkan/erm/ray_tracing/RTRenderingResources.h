#pragma once

#include "erm/ray_tracing/RTRenderConfigs.h"
#include "erm/ray_tracing/RTTlas.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {
	class Device;
	class IRenderer;
	class RTBlas;
	class RTPipelineResources;
	class DeviceBuffer;
	struct RTRenderData;
} // namespace erm

namespace erm {

	class RTRenderingResources
	{
	public:
		RTRenderingResources(
			Device& device,
			IRenderer& renderer);
		~RTRenderingResources();

		void Update(std::vector<RTRenderData*>& renderData, uint32_t imageIndex);
		vk::CommandBuffer UpdateCommandBuffer(std::vector<RTRenderData*>& renderData, uint32_t imageIndex);

		void Refresh();

	private:
		void BuildBlas(std::vector<RTBlas*> toBuild, vk::BuildAccelerationStructureFlagsKHR flags);
		void UpdateTopLevelAS(std::vector<RTRenderData*> data, vk::BuildAccelerationStructureFlagsKHR flags);
		RTPipelineResources& GetOrCreatePipelineResources(const RTRenderData& renderData);

		void Reload();
		void Cleanup();

		void CreateDescriptorPool();
		void CreateCommandBuffers();

		Device& mDevice;
		IRenderer& mRenderer;

		vk::UniqueDescriptorPool mDescriptorPool;

		RTTlas mTopLevelAS;
		std::vector<vk::CommandBuffer> mCommandBuffers;

		std::unique_ptr<DeviceBuffer> mInstancesBuffer;
		std::unique_ptr<RTPipelineResources> mPipelineResources;
	};

} // namespace erm
