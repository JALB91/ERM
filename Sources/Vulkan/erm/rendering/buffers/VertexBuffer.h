#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"

namespace erm {

	class VertexBuffer : public DeviceBuffer
	{
	public:
		VertexBuffer(Device& device, size_t size, void* data);

		void Bind(const vk::CommandBuffer& commandBuffer) const;
	};

} // namespace erm
