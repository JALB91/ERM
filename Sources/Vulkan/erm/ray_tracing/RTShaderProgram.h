#pragma once

#include "erm/rendering/shaders/VulkanShaderProgram.h"

namespace erm {

class RTShaderProgram : public VulkanShaderProgram
{
public:
	RTShaderProgram(Device& device, const char* shaderPath);
};

} // namespace erm
