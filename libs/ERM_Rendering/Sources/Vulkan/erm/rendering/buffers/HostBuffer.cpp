#include "erm/rendering/buffers/HostBuffer.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/utils/VkUtils.h"

namespace erm {

HostBuffer::HostBuffer(
	Device& device,
	u64 size,
	BufferUsageFlags buf)
	: IBuffer(device, size, buf, MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT)
{}

void HostBuffer::update(const void* data, const BufferInfo& info /*= {}*/) const
{
	void* mappedData;
	ERM_VK_CHECK_AND_ASSIGN(mappedData, mDevice->mapMemory(mBufferMemory.get(), info.mOffset, info.mStride));
	memcpy(mappedData, data, info.mStride == VK_WHOLE_SIZE ? mBufferSize : info.mStride);
	mDevice->unmapMemory(mBufferMemory.get());
}

} // namespace erm
