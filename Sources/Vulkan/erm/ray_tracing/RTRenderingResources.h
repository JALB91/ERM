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
			IRenderer& renderer,
			const RTRenderConfigs& renderConfigs);
		~RTRenderingResources();

		inline const RTRenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }

		void Update(std::vector<RTRenderData*>& renderData, uint32_t imageIndex);
		vk::CommandBuffer UpdateCommandBuffer(std::vector<RTRenderData*>& renderData, uint32_t imageIndex);

		void Refresh();

	private:
		void BuildBlas(std::vector<RTBlas*>& toBuild, vk::BuildAccelerationStructureFlagsKHR flags);
		void UpdateTopLevelAS(std::vector<RTRenderData*>& data, bool forceUpdate, vk::BuildAccelerationStructureFlagsKHR flags);
		bool UpdateInstances(std::vector<RTRenderData*>& data);

		void Reload();
		void Cleanup();

		void CreateDescriptorPool();
		void CreateCommandBuffers();

		Device& mDevice;
		IRenderer& mRenderer;
		const RTRenderConfigs mRenderConfigs;

		vk::UniqueDescriptorPool mDescriptorPool;

		RTTlas mTopLevelAS;
		std::vector<vk::CommandBuffer> mCommandBuffers;

		std::vector<std::pair<RTRenderData*, RTBlas*>> mData;
		std::unique_ptr<DeviceBuffer> mInstancesBuffer;
		std::unique_ptr<RTPipelineResources> mPipelineResources;
	};

} // namespace erm
