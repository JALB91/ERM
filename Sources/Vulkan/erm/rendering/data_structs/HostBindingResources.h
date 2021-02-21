#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/IBindingResources.h"

#include <deque>

namespace erm {

	class HostBindingResources : public IBindingResources
	{
	public:
		HostBindingResources(
			Device& device,
			IRenderer& renderer,
			uint32_t targetSet,
			const vk::DescriptorPool& descriptorPool,
			const RenderConfigs& renderConfigs,
			const vk::DescriptorSetLayout& descriptorSetLayout);

		const vk::DescriptorSet GetDescriptorSet() const override;

		void UpdateResources(vk::CommandBuffer& cmd, RenderData& data) override;
		void PostDraw() override;

	private:
		using UniformBuffers = std::map<UboId, UniformBuffer<HostBuffer>>;
		using SwapChainUniformBuffers = std::deque<UniformBuffers>;

	private:
		void CreateUniformBuffers(const std::vector<UboData>& ubosData);

		SwapChainUniformBuffers mUniformBuffers;
		uint32_t mCurrentBufferIndex;

	};

}