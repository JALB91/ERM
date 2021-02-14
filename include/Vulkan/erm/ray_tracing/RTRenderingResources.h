#pragma once

#include "erm/ray_tracing/RTRenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
	class Device;
	class IRenderer;
	struct RenderData;
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

		vk::CommandBuffer UpdateCommandBuffer(std::vector<RenderData*>& renderData, uint32_t imageIndex);

		void Refresh();

	private:
		void Reload();
		void Cleanup();

		void CreatePipeline();
		void CreateDescriptorPool();
		void CreateCommandBuffers();

		Device& mDevice;
		IRenderer& mRenderer;
		const RTRenderConfigs mRenderConfigs;

		vk::DescriptorPool mDescriptorPool;
		vk::UniquePipelineLayout mPipelineLayout;
		vk::UniquePipeline mPipeline;

		vk::DescriptorSetLayout mDescriptorSetLayout;

		std::vector<vk::CommandBuffer> mCommandBuffers;
	};

} // namespace erm
