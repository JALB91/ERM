#include "erm/rendering/buffers/VertexBuffer.h"

#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

	VertexBuffer::VertexBuffer(
		Device& device,
		size_t size,
		void* data)
		: DeviceBuffer(device, size, vk::BufferUsageFlagBits::eVertexBuffer)
	{
		vk::CommandBuffer cmd = VkUtils::BeginSingleTimeCommands(mDevice.GetCommandPool(), mDevice.GetVkDevice());
		Update(cmd, data);
		VkUtils::EndSingleTimeCommands(mDevice.GetGraphicsQueue(), mDevice.GetCommandPool(), mDevice.GetVkDevice(), cmd);
	}

	void VertexBuffer::Bind(const vk::CommandBuffer& commandBuffer) const
	{
		vk::Buffer vertexBuffers[] = {mBuffer};
		vk::DeviceSize offsets[] = {0};
		commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	}

} // namespace erm
