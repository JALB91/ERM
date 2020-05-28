#include "erm/rendering/buffers/IndexBuffer.h"

#include "erm/rendering/Device.h"
#include "erm/utils/VkUtils.h"

#include <vulkan/vulkan.hpp>

namespace erm {

	IndexBuffer::IndexBuffer(Device& device, void* data, size_t size, uint32_t count)
		: mDevice(device)
		, mCount(count)
	{
		VkUtils::CreateDeviceLocalBuffer(
			mDevice.GetTransferQueue(),
			mDevice.GetCommandPool(),
			mDevice.GetVkPhysicalDevice(),
			mDevice.GetVkDevice(),
			vk::BufferUsageFlagBits::eIndexBuffer,
			size,
			data,
			mBuffer,
			mBufferMemory);
	}

	IndexBuffer::~IndexBuffer()
	{
		mDevice->freeMemory(mBufferMemory);
		mDevice->destroyBuffer(mBuffer);
	}

	void IndexBuffer::Bind(const vk::CommandBuffer& commandBuffer) const
	{
		commandBuffer.bindIndexBuffer(mBuffer, 0, vk::IndexType::eUint32);
	}

} // namespace erm
