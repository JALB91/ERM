#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/IBindingResources.h"
#include "erm/rendering/renderer/Renderer.h"

#include <erm/math/Types.h>

#include <vector>

namespace erm {

class HostBindingResources : public IBindingResources
{
public:
	HostBindingResources(
		Device& device,
		Renderer& renderer,
		u32 targetSet,
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs,
		vk::DescriptorPool descriptorPool,
		vk::DescriptorSetLayout descriptorSetLayout);

	vk::DescriptorSet getDescriptorSet() const override;

	void updateResources(vk::CommandBuffer cmd, IRenderData& data) override;

private:
	using SwapChainUniformBuffers = std::array<UniformBuffersMap<HostBuffer>, Renderer::kMaxFramesInFlight>;

private:
	void createUniformBuffers(const std::vector<UboData>& ubosData);

	SwapChainUniformBuffers mUniformBuffers;
};

} // namespace erm
