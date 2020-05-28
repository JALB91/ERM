#pragma once

#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <map>
#include <vector>

namespace erm {
	class Device;
	struct RenderConfigs;
	struct UniformBuffer;
} // namespace erm

namespace erm {

	class RenderingResources
	{
	public:
		RenderingResources(
			Device& device,
			const RenderConfigs& renderConfigs,
			const std::vector<vk::ImageView>& swapChainImageViews,
			vk::Format swapChainImageFormat,
			vk::Sampler textureSampler);
		~RenderingResources();

		inline const RenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }
		inline const std::vector<vk::CommandBuffer>& GetCommandBuffers() const { return mCommandBuffers; }

		void UpdateUniformBuffer(uint32_t index, const UniformBuffer& ub) const;

	private:
		void CreateRenderPass();
		void CreatePipeline();
		void CreateDepthResources();
		void CreateFramebuffers();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateCommandBuffers();

	public:
		Device& mDevice;
		const RenderConfigs mRenderConfigs;

		const std::vector<vk::ImageView>& mSwapChainImageViews;
		vk::Format mSwapChainImageFormat;
		vk::Sampler mTextureSampler;

		vk::RenderPass mRenderPass;
		vk::PipelineLayout mPipelineLayout;
		vk::Pipeline mPipeline;

		vk::Image mDepthImage;
		vk::DeviceMemory mDepthImageMemory;
		vk::ImageView mDepthImageView;

		std::vector<vk::Framebuffer> mSwapChainFramebuffers;

		std::vector<vk::Buffer> mUniformBuffers;
		std::vector<vk::DeviceMemory> mUniformBuffersMemory;

		vk::DescriptorPool mDescriptorPool;
		vk::DescriptorSetLayout mDescriptorSetLayout;
		std::vector<vk::DescriptorSet> mDescriptorSets;

		std::vector<vk::CommandBuffer> mCommandBuffers;
	};

} // namespace erm
