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
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs,
		const IRenderData& renderData,
		vk::DescriptorPool descriptorPool,
		vk::DescriptorSetLayout descriptorSetLayout
#ifdef ERM_RAY_TRACING_ENABLED
		,
		const vk::AccelerationStructureKHR* as = nullptr
#endif
	);

	vk::DescriptorSet GetDescriptorSet() const override;

	void UpdateResources(vk::CommandBuffer cmd, IRenderData& data) override;

private:
	void CreateUniformBuffers(const std::vector<UboData>& ubosData);

	UniformBuffersMap<DeviceBuffer> mUniformBuffersMap;
};

} // namespace erm
