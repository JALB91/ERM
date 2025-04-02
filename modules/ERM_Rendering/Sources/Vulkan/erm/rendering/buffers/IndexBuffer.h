#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"

#include <erm/math/Types.h>

namespace erm {

class IndexBuffer : public DeviceBuffer
{
public:
	IndexBuffer(Device& device, u64 size, u32 count);

	inline void setIndicesCount(u32 count) { mCount = count; }
	inline u32 getIndicesCount() const { return mCount; }

private:
	u32 mCount;
};

} // namespace erm
