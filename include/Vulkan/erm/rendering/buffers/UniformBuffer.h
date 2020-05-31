#pragma once

#include "erm/math/mat.h"

#include "erm/rendering/buffers/Buffer.h"

namespace erm {

	struct UniformBufferObject
	{
		alignas(16) erm::math::mat4 mMVP;
	};

	class UniformBuffer : public Buffer
	{
	public:
		UniformBuffer(Device& device, void* data, size_t size);

		void Update(void* data) const;
	};

} // namespace erm
