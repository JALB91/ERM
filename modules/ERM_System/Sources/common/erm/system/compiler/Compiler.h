#pragma once

namespace erm {

enum class Compiler
{
	MSVC,
	CLANG,
	APPLE_CLANG,
	GNU
};

#ifdef ERM_HOST_OSX
constexpr Compiler kDefaultCompiler = Compiler::APPLE_CLANG;
#elifdef ERM_HOST_WIN
constexpr Compiler kDefaultCompiler = Compiler::MSVC;
#elifdef ERM_HOST_LINUX
constexpr Compiler kDefaultCompiler = Compiler::GNU;
#else
#error "Host platform not supported"
#endif

}

