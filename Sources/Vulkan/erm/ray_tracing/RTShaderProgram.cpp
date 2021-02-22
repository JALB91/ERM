#include "erm/ray_tracing/RTShaderProgram.h"

namespace erm {

	RTShaderProgram::RTShaderProgram(Device& device, const char* shaderPath)
		: IShaderProgram(device, shaderPath)
	{
		//UpdateShaderData(ShaderType::RT_ANY_HIT);
		UpdateShaderData(ShaderType::RT_CLOSEST_HIT);
		//UpdateShaderData(ShaderType::RT_INTERSECTION);
		UpdateShaderData(ShaderType::RT_MISS);
		UpdateShaderData(ShaderType::RT_RAY_GEN);
		UpdateBindingData();
	}

} // namespace erm
