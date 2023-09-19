#pragma once

#include <erm/math/Types.h>

namespace erm {

enum class DrawMode : u8
{
	LINES,
	LINE_STRIP,
	TRIANGLES,
	TRIANGLE_FAN,
	TRIANGLE_STRIP
};

}