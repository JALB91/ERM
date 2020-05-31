#include "erm/rendering/buffers/UniformBuffer.h"

#include "erm/rendering/Device.h"

#include "erm/utils/VkUtils.h"

namespace erm {

	UniformBuffer::UniformBuffer(Device& device, void* data, size_t size)
		: Buffer(device, size)
	{
		VkUtils::CreateBuffer(
			device.GetVkPhysicalDevice(),
			device.GetVkDevice(),
			size,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			mBuffer,
			mBufferMemory);

		Update(data);
	}

	void UniformBuffer::Update(void* data) const
	{
		void* mappedData = mDevice->mapMemory(mBufferMemory, 0, mBufferSize);
		memcpy(mappedData, data, mBufferSize);
		mDevice->unmapMemory(mBufferMemory);
	}

} // namespace erm
