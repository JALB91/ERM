#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/IBindingResources.h"
#include "erm/rendering/renderer/IRenderer.h"

#include <vector>

namespace erm {

class HostBindingResources : public IBindingResources
{
public:
	HostBindingResources(
		Device& device,
		IRenderer& renderer,
		uint32_t targetSet,
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs,
		vk::DescriptorPool descriptorPool,
		vk::DescriptorSetLayout descriptorSetLayout);

	vk::DescriptorSet GetDescriptorSet() const override;

	void UpdateResources(vk::CommandBuffer cmd, IRenderData& data) override;

private:
	using SwapChainUniformBuffers = std::array<UniformBuffersMap<HostBuffer>, IRenderer::kMaxFramesInFlight>;

private:
	void CreateUniformBuffers(const std::vector<UboData>& ubosData);

	SwapChainUniformBuffers mUniformBuffers;
};

} // namespace erm
