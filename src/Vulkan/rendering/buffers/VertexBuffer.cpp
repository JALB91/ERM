#include "erm/rendering/buffers/VertexBuffer.h"

#include "erm/rendering/Device.h"

#include "erm/utils/VkUtils.h"

namespace erm {

	VertexBuffer::VertexBuffer(
		Device& device,
		void* data,
		size_t size)
		: Buffer(device, size)
	{
		VkUtils::CreateDeviceLocalBuffer(
			device.GetTransferQueue(),
			device.GetCommandPool(),
			device.GetVkPhysicalDevice(),
			device.GetVkDevice(),
			vk::BufferUsageFlagBits::eVertexBuffer,
			size,
			data,
			mBuffer,
			mBufferMemory);
	}

	void VertexBuffer::Bind(const vk::CommandBuffer& commandBuffer) const
	{
		vk::Buffer vertexBuffers[] = {mBuffer};
		vk::DeviceSize offsets[] = {0};
		commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	}

} // namespace erm
