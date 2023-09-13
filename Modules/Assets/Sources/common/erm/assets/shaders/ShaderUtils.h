#pragma once

#include "erm/assets/enums/ShaderType.h"

#include <string>
#include <string_view>

namespace erm::ShaderUtils {

extern std::string GetShaderFilename(
	std::string_view shader,
	uint32_t index,
	ShaderType type);
extern std::string GetSuffixForShaderIndex(uint32_t index);
extern std::string_view GetExtensionForShaderType(ShaderType shaderType);

} // namespace erm::ShaderUtils
