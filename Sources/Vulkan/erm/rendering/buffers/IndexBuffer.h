#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"

namespace erm {

	class IndexBuffer : public DeviceBuffer
	{
	public:
		IndexBuffer(Device& device, size_t size, void* data, uint32_t count);

		void Bind(const vk::CommandBuffer& commandBuffer) const;

		inline uint32_t GetCount() const { return mCount; }

	private:
		const uint32_t mCount;
	};

} // namespace erm
