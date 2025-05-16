#pragma once

namespace erm {

enum class CMakeGenerator
{
	UNIXMK,
	XCODE,
	NINJA,
	VS
};

#ifdef ERM_HOST_OSX
constexpr CMakeGenerator kDefaultCMakeGenerator = CMakeGenerator::XCODE;
#elifdef ERM_HOST_WIN
constexpr CMakeGenerator kDefaultCMakeGenerator = CMakeGenerator::VS;
#elifdef ERM_HOST_LINUX
constexpr CMakeGenerator kDefaultCMakeGenerator = CMakeGenerator::UNIXMK;
#else
#error "Host platform not supported"
#endif

}
