#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"

namespace erm {

class IndexBuffer : public DeviceBuffer
{
public:
	IndexBuffer(Device& device, size_t size, uint32_t count);

	inline uint32_t GetCount() const { return mCount; }

private:
	uint32_t mCount;
};

} // namespace erm
