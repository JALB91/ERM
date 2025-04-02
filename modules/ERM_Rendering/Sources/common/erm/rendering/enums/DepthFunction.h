#pragma once

#include <erm/math/Types.h>

namespace erm {

enum class DepthFunction : u8
{
	ALWAYS,
	NEVER,
	LESS,
	EQUAL,
	LEQUAL,
	GREATER,
	NOT_EQUAL,
	GEQUAL
};

}
