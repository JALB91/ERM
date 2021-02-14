#include "erm/rendering/buffers/IndexBuffer.h"

#include "erm/rendering/Device.h"

#include "erm/utils/VkUtils.h"

namespace erm {

	IndexBuffer::IndexBuffer(Device& device, void* data, size_t size, uint32_t count)
		: Buffer(device, size)
		, mCount(count)
	{
		VkUtils::CreateDeviceLocalBuffer(
			device.GetTransferQueue(),
			device.GetCommandPool(),
			device.GetVkPhysicalDevice(),
			device.GetVkDevice(),
			vk::BufferUsageFlagBits::eIndexBuffer,
			size,
			data,
			mBuffer,
			mBufferMemory);
	}

	void IndexBuffer::Bind(const vk::CommandBuffer& commandBuffer) const
	{
		commandBuffer.bindIndexBuffer(mBuffer, 0, vk::IndexType::eUint32);
	}

} // namespace erm
