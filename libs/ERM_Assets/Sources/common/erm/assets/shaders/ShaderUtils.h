#pragma once

#include "erm/assets/enums/ShaderType.h"

#include <erm/math/Types.h>

#include <string>
#include <string_view>

namespace erm::ShaderUtils {

extern std::string getShaderFilename(
	std::string_view shader,
	u32 index,
	ShaderType type);
extern std::string getSuffixForShaderIndex(u32 index);
extern std::string_view getExtensionForShaderType(ShaderType shaderType);

} // namespace erm::ShaderUtils
