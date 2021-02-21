#include "erm/rendering/buffers/HostBuffer.h"

#include "erm/rendering/Device.h"

namespace erm {

	HostBuffer::HostBuffer(
		Device& device,
		size_t size,
		vk::BufferUsageFlags buf)
		: IBuffer(device, size, buf, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
	{}

	void HostBuffer::Update(void* data) const
	{
		void* mappedData = mDevice->mapMemory(mBufferMemory, 0, mBufferSize);
		memcpy(mappedData, data, mBufferSize);
		mDevice->unmapMemory(mBufferMemory);
	}

}