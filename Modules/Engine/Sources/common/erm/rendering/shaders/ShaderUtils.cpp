#include "erm/rendering/shaders/ShaderUtils.h"

#include "erm/utils/Utils.h"

namespace erm::ShaderUtils {

std::string GetShaderFilename(
	const std::string& shader,
	uint32_t index,
	ShaderType type)
{
	return shader + GetSuffixForShaderIndex(index) + GetExtensionForShaderType(type);
}

std::string GetSuffixForShaderIndex(uint32_t index)
{
	if (index == 0)
		return "";
	else
		return "_" + std::to_string(index);
}

const char* GetExtensionForShaderType(ShaderType shaderType)
{
	switch (shaderType)
	{
		case ShaderType::VERTEX:
			return ".vert";
		case ShaderType::FRAGMENT:
			return ".frag";
#ifdef ERM_RAY_TRACING_ENABLED
		case ShaderType::RT_ANY_HIT:
			return ".rahit";
		case ShaderType::RT_CALLABLE:
			return ".rcall";
		case ShaderType::RT_CLOSEST_HIT:
			return ".rchit";
		case ShaderType::RT_INTERSECTION:
			return ".rint";
		case ShaderType::RT_MISS:
			return ".rmiss";
		case ShaderType::RT_RAY_GEN:
			return ".rgen";
#endif
		default:
			ERM_ASSERT(false);
			return "";
	}
}

} // namespace erm::ShaderUtils
