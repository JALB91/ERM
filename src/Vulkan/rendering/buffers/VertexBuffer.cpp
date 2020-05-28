#include "erm/rendering/buffers/VertexBuffer.h"

#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

	VertexBuffer::VertexBuffer(
		Device& device,
		void* data,
		size_t size)
		: mDevice(device)
	{
		VkUtils::CreateDeviceLocalBuffer(
			mDevice.GetTransferQueue(),
			mDevice.GetCommandPool(),
			mDevice.GetVkPhysicalDevice(),
			mDevice.GetVkDevice(),
			vk::BufferUsageFlagBits::eVertexBuffer,
			size,
			data,
			mBuffer,
			mBufferMemory);
	}

	VertexBuffer::~VertexBuffer()
	{
		mDevice->freeMemory(mBufferMemory);
		mDevice->destroyBuffer(mBuffer);
	}

	void VertexBuffer::Bind(const vk::CommandBuffer& commandBuffer) const
	{
		vk::Buffer vertexBuffers[] = {mBuffer};
		vk::DeviceSize offsets[] = {0};
		commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	}

} // namespace erm
