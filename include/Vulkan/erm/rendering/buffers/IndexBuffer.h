#pragma once

#include "erm/rendering/buffers/Buffer.h"

namespace erm {

	class IndexBuffer : public Buffer
	{
	public:
		IndexBuffer(Device& device, void* data, size_t size, uint32_t count);

		void Bind(const vk::CommandBuffer& commandBuffer) const;

		inline uint32_t GetCount() const { return mCount; }

	private:
		const uint32_t mCount;
	};

} // namespace erm
