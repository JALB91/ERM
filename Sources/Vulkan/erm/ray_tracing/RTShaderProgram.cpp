#include "erm/ray_tracing/RTShaderProgram.h"

namespace erm {

RTShaderProgram::RTShaderProgram(Device& device, const char* shaderPath)
	: VulkanShaderProgram(device, shaderPath)
{
	//UpdateShaderData(ShaderType::RT_ANY_HIT);
	UpdateShadersData(ShaderType::RT_CLOSEST_HIT);
	//UpdateShaderData(ShaderType::RT_INTERSECTION);
	UpdateShadersData(ShaderType::RT_MISS);
	UpdateShadersData(ShaderType::RT_RAY_GEN);
	UpdateBindingData();
}

} // namespace erm
