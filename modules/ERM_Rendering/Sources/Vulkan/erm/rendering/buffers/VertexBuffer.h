#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"

#include <erm/math/Types.h>

namespace erm {

class VertexBuffer : public DeviceBuffer
{
public:
	VertexBuffer(Device& device, u64 size);
};

} // namespace erm
