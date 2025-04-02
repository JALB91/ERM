#pragma once

#include "erm/rendering/Macros.h"

#include <erm/math/Types.h>

namespace erm {

struct BufferInfo
{
	BufferInfo(
		u64 offset = 0,
		u64 stride = ERM_WHOLE_SIZE)
		: mOffset(offset)
		, mStride(stride)
	{}

	u64 mOffset;
	u64 mStride;
};

} // namespace erm