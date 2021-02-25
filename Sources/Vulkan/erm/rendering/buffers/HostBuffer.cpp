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
		Update(data, 0, mBufferSize);
	}

	void HostBuffer::Update(void* data, vk::DeviceSize offset, vk::DeviceSize size) const
	{
		void* mappedData = mDevice->mapMemory(mBufferMemory.get(), offset, size);
		memcpy(mappedData, data, size);
		mDevice->unmapMemory(mBufferMemory.get());
	}

} // namespace erm