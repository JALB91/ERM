#pragma once

namespace erm {

enum class HostPlatform
{
	WINDOWS,
	LINUX,
	OSX
};

#if defined(ERM_WINDOWS)
constexpr auto kHostPlatform = HostPlatform::WINDOWS;
#elif defined(ERM_OSX)
constexpr auto kHostPlatform = HostPlatform::OSX;
#elif defined(ERM_LINUX)
constexpr auto kHostPlatform = HostPlatform::LINUX;
#else
#error "Host platform not supported"
#endif

}

