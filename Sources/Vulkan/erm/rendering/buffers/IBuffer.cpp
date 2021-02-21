#include "erm/rendering/buffers/IBuffer.h"

#include "erm/rendering/Device.h"

#include "erm/utils/VkUtils.h"

namespace erm {

	IBuffer::IBuffer(
		Device& device,
		size_t size,
		vk::BufferUsageFlags buf,
		vk::MemoryPropertyFlags mpf)
		: mDevice(device)
		, mBufferSize(size)
	{
		VkUtils::CreateBuffer(
			device.GetVkPhysicalDevice(),
			device.GetVkDevice(),
			size,
			buf,
			mpf,
			mBuffer,
			mBufferMemory);
	}

	IBuffer::~IBuffer()
	{
		if (mBufferMemory && mBuffer)
		{
			mDevice->freeMemory(mBufferMemory);
			mDevice->destroyBuffer(mBuffer);
		}
	}

	IBuffer::IBuffer(IBuffer&& other)
		: mDevice(other.mDevice)
		, mBufferSize(other.mBufferSize)
		, mBuffer(std::move(other.mBuffer))
		, mBufferMemory(std::move(other.mBufferMemory))
	{
		other.mBuffer = nullptr;
		other.mBufferMemory = nullptr;
	}

} // namespace erm
