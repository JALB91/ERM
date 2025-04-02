#pragma once

#include "erm/rendering/buffers/IBuffer.h"

namespace erm {

class HostBuffer : public IBuffer
{
public:
	HostBuffer(
		Device& device,
		u64 size,
		BufferUsageFlags buf);

	void update(const void* data, const BufferInfo& info = {}) const override;
};

} // namespace erm