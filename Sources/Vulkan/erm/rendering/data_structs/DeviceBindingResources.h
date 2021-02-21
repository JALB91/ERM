#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/IBindingResources.h"

namespace erm {
	
	class DeviceBindingResources : public IBindingResources
	{
	public:
		DeviceBindingResources(
			Device& device,
			IRenderer& renderer,
			uint32_t targetSet,
			const vk::DescriptorPool& descriptorPool,
			const RenderConfigs& renderConfigs,
			const vk::DescriptorSetLayout& descriptorSetLayout);

		const vk::DescriptorSet GetDescriptorSet() const override;

		void UpdateResources(vk::CommandBuffer& cmd, RenderData& data) override;

	private:
		using UniformBuffers = std::map<UboId, UniformBuffer<DeviceBuffer>>;
	
	private:
		void CreateUniformBuffers(const std::vector<UboData>& ubosData);

		UniformBuffers mUniformBuffers;

	};

}