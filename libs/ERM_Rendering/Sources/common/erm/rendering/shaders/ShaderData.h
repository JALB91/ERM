#pragma once

#include <erm/math/Types.h>

#include <spirv_cpp.hpp>

#include <memory>
#include <string>
#include <vector>

namespace erm {

struct ShaderData
{
	std::string mShaderSource;
	std::vector<u8> mShaderByteCode;
	std::unique_ptr<spirv_cross::Compiler> mShaderCompiler;
};

} // namespace erm