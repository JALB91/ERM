#include "erm/rendering/buffers/IndexBuffer.h"

#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

	IndexBuffer::IndexBuffer(Device& device, size_t size, void* data, uint32_t count)
		: DeviceBuffer(device, size, vk::BufferUsageFlagBits::eIndexBuffer
#ifdef ERM_RAY_TRACING_ENABLED
						   | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR
#endif
					   )
		, mCount(count)
	{
		vk::CommandBuffer cmd = VkUtils::BeginSingleTimeCommands(mDevice.GetCommandPool(), mDevice.GetVkDevice());
		Update(cmd, data);
		VkUtils::EndSingleTimeCommands(mDevice.GetGraphicsQueue(), mDevice.GetCommandPool(), mDevice.GetVkDevice(), cmd);
	}

	void IndexBuffer::Bind(const vk::CommandBuffer& commandBuffer) const
	{
		commandBuffer.bindIndexBuffer(mBuffer, 0, vk::IndexType::eUint32);
	}

} // namespace erm
