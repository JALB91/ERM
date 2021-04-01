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
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs,
		const IRenderData& renderData,
		const vk::DescriptorSetLayout& descriptorSetLayout
#ifdef ERM_RAY_TRACING_ENABLED
		,
		const vk::AccelerationStructureKHR* as = nullptr
#endif
	);

	const vk::DescriptorSet GetDescriptorSet() const override;

	void UpdateResources(vk::CommandBuffer& cmd, IRenderData& data) override;

private:
	using UniformBuffers = std::map<UboId, UniformBuffer<DeviceBuffer>>;

private:
	void CreateUniformBuffers(const std::vector<UboData>& ubosData);

	UniformBuffers mUniformBuffers;
};

} // namespace erm