#include "erm/assets/shaders/ShaderUtils.h"

#include <erm/utils/Utils.h>

#include <format>
#include <utility>

namespace erm::ShaderUtils {

std::string getShaderFilename(
	std::string_view shader,
	u32 index,
	ShaderType type)
{
	return std::format("{}{}.{}", shader, index == 0 ? "" : "_", getExtensionForShaderType(type));
}

std::string_view getExtensionForShaderType(ShaderType shaderType)
{
	switch (shaderType)
	{
		case ShaderType::VERTEX:
			return "vert";
		case ShaderType::FRAGMENT:
			return "frag";
#ifdef ERM_RAY_TRACING_ENABLED
		case ShaderType::RT_ANY_HIT:
			return "rahit";
		case ShaderType::RT_CALLABLE:
			return "rcall";
		case ShaderType::RT_CLOSEST_HIT:
			return "rchit";
		case ShaderType::RT_INTERSECTION:
			return "rint";
		case ShaderType::RT_MISS:
			return "rmiss";
		case ShaderType::RT_RAY_GEN:
			return "rgen";
#endif
		default:
			std::unreachable();
	}
}

} // namespace erm::ShaderUtils
