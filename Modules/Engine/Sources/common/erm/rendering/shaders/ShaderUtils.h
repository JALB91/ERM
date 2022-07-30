#pragma once

#include "erm/rendering/enums/ShaderType.h"

#include <string>

namespace erm::ShaderUtils {

extern std::string GetShaderFilename(
	const std::string& shader,
	uint32_t index,
	ShaderType type);
extern std::string GetSuffixForShaderIndex(uint32_t index);
extern const char* GetExtensionForShaderType(ShaderType shaderType);

} // namespace erm::ShaderUtils