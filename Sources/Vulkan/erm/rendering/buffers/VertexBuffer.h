#pragma once

#include "erm/rendering/buffers/Buffer.h"

namespace erm {

	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer(Device& device, void* data, size_t size);

		void Bind(const vk::CommandBuffer& commandBuffer) const;
	};

} // namespace erm
