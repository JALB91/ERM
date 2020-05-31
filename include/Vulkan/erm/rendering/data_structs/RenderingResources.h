#pragma once

#include "erm/rendering/data_structs/BindingResources.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
	class Device;
	class Renderer;
	struct RenderConfigs;
	struct RenderData;
} // namespace erm

namespace erm {

	class RenderingResources
	{
	public:
		RenderingResources(
			Device& device,
			Renderer& renderer,
			const RenderConfigs& renderConfigs);
		~RenderingResources();

		void CreateDescriptorPool(vk::DescriptorPoolCreateInfo& info);
		void UpdateRenderingResources(std::vector<RenderData*>& renderData, uint32_t imageIndex);

	private:
		void CreateRenderPass();
		void CreatePipeline();
		void CreateDepthResources();
		void CreateFramebuffers();
		void CreateDescriptorPool();
		void CreateCommandBuffers();

	public:
		Device& mDevice;
		Renderer& mRenderer;
		const RenderConfigs mRenderConfigs;

		vk::RenderPass mRenderPass;
		vk::PipelineLayout mPipelineLayout;
		vk::Pipeline mPipeline;

		std::vector<vk::Framebuffer> mSwapChainFramebuffers;

		vk::DescriptorPool mDescriptorPool;
		vk::DescriptorSetLayout mDescriptorSetLayout;

		std::vector<BindingResources> mBindingResources;
		std::vector<vk::CommandBuffer> mCommandBuffers;
	};

} // namespace erm
