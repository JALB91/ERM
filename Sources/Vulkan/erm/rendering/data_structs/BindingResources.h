#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/data_structs/UniformBufferObject.h"

#include <vulkan/vulkan.hpp>

#include <deque>
#include <map>
#include <memory>
#include <vector>

namespace erm {
	class Device;
	class Renderer;
	struct RenderData;
} // namespace erm

namespace erm {

	using UniformBuffers = std::map<UboId, UniformBuffer>;
	using SwapChainsUniformBuffers = std::deque<UniformBuffers>;

	class BindingResources
	{
	public:
		BindingResources(
			Device& device,
			Renderer& renderer,
			const vk::DescriptorPool& descriptorPool,
			const vk::DescriptorSetLayout& descriptorSetLayout,
			const RenderConfigs& renderConfigs);
		~BindingResources();

		BindingResources(const BindingResources&) = delete;
		BindingResources& operator=(const BindingResources&) = delete;

		BindingResources(BindingResources&& other);
		BindingResources& operator=(BindingResources&&) = delete;

		void UpdateResources(RenderData& data, uint32_t index);

		inline vk::DescriptorSet* GetDescriptorSet(uint32_t index) { return &mDescriptorSets[index]; }

	private:
		Device& mDevice;
		Renderer& mRenderer;
		const vk::DescriptorPool& mDescriptorPool;
		const RenderConfigs mRenderConfigs;
		std::vector<vk::DescriptorSet> mDescriptorSets;
		SwapChainsUniformBuffers mUniformBuffers;
	};

} // namespace erm
