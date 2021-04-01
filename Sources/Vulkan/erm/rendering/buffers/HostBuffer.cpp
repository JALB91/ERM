#include "erm/rendering/buffers/HostBuffer.h"

#include "erm/rendering/Device.h"

namespace erm {

HostBuffer::HostBuffer(
	Device& device,
	size_t size,
	vk::BufferUsageFlags buf)
	: IBuffer(device, size, buf, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
{}

void HostBuffer::Update(const void* data, const BufferInfo& info /*= {}*/) const
{
	void* mappedData = mDevice->mapMemory(mBufferMemory.get(), info.mOffset, info.mStride);
	memcpy(mappedData, data, info.mStride == VK_WHOLE_SIZE ? mBufferSize : info.mStride);
	mDevice->unmapMemory(mBufferMemory.get());
}

} // namespace erm