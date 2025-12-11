#pragma once

#include "erm/assets/enums/ShaderType.h"

#include <erm/math/Types.h>

#include <string>
#include <string_view>

namespace erm::ShaderUtils {

std::string getShaderFilename(
	std::string_view shader,
	u32 index,
	ShaderType type);
std::string_view getExtensionForShaderType(ShaderType shaderType);

} // namespace erm::ShaderUtils
