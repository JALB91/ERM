#pragma once

#include <spirv_cpp.hpp>

#include <memory>
#include <string>
#include <vector>

namespace erm {

	struct ShaderData
	{
		std::string mShaderSource;
		std::vector<char> mShaderByteCode;
		std::unique_ptr<spirv_cross::Compiler> mShaderCompiler;
	};

} // namespace erm