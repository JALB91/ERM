#pragma once

#include "erm/rendering/buffers/IBuffer.h"

namespace erm {

	class HostBuffer : public IBuffer
	{
	public:
		HostBuffer(
			Device& device,
			size_t size,
			vk::BufferUsageFlags buf);

		void Update(void* data) const;
	};

} // namespace erm