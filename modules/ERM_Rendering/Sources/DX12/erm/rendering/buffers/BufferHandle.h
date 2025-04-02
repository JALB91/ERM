#pragma once

#include "erm/rendering/buffers/BufferInfo.h"

namespace erm {

struct BufferHandle
{
	BufferHandle(void* buffer, const BufferInfo& info)
		: mBuffer(buffer)
		, mInfo(info)
	{}

	void* mBuffer;
	BufferInfo mInfo;
};

} // namespace erm