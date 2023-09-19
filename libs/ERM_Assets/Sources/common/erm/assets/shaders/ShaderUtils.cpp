#include "erm/assets/shaders/ShaderUtils.h"

#include "erm/utils/Utils.h"

namespace erm::ShaderUtils {

std::string getShaderFilename(
	std::string_view shader,
	u32 index,
	ShaderType type)
{
	return std::string(shader) + getSuffixForShaderIndex(index) + std::string(getExtensionForShaderType(type));
}

std::string getSuffixForShaderIndex(u32 index)
{
	if (index == 0)
		return "";
	else
		return "_" + std::to_string(index);
}

std::string_view getExtensionForShaderType(ShaderType shaderType)
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
