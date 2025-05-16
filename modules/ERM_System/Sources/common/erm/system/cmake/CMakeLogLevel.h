#pragma once

namespace erm {

enum class CMakeLogLevel
{
	ERROR,
	WARNING,
	NOTICE,
	STATUS,
	VERBOSE,
	DEBUG,
	TRACE
};

constexpr auto kDefaultCMakeLogLevel = CMakeLogLevel::STATUS;

}
