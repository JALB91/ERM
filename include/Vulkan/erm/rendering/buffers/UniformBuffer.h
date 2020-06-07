#pragma once

#include "erm/rendering/buffers/Buffer.h"

namespace erm {

	class UniformBuffer : public Buffer
	{
	public:
		UniformBuffer(Device& device, void* data, size_t size);
		UniformBuffer(Device& device, size_t size);

		void Update(void* data) const;
	};

} // namespace erm
