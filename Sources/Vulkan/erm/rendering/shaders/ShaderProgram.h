#pragma once

#include "erm/rendering/shaders/VulkanShaderProgram.h"

namespace erm {

class ShaderProgram : public VulkanShaderProgram
{
public:
	ShaderProgram(Device& device, const char* shaderPath);

	vk::VertexInputBindingDescription GetVertexBindingDescription();
	std::vector<vk::VertexInputAttributeDescription> GetVertexAttributeDescriptions();
};

} // namespace erm
