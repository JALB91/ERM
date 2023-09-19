#include "erm/rendering/buffers/DeviceBuffer.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/HostBuffer.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/utils/Utils.h>

namespace erm {

DeviceBuffer::DeviceBuffer(
	Device& device,
	u64 size,
	BufferUsageFlags buf)
	: IBuffer(device, size, buf | BufferUsage::TRANSFER_DST, MemoryProperty::DEVICE_LOCAL)
{}

DeviceBuffer::~DeviceBuffer() = default;

DeviceBuffer::DeviceBuffer(DeviceBuffer&& other) noexcept
	: IBuffer(std::move(other))
	, mStagingBuffer(std::move(other.mStagingBuffer))
{}

DeviceBuffer& DeviceBuffer::operator=(DeviceBuffer&& other) noexcept
{
	if (this == &other)
		return *this;

	IBuffer::operator=(std::move(other));
	mStagingBuffer = std::move(other.mStagingBuffer);

	return *this;
}

void DeviceBuffer::update(const void* data, const BufferInfo& info /*= {}*/) const
{
	vk::CommandBuffer cmd = VkUtils::beginSingleTimeCommands(mDevice);
	update(cmd, data, info);
	VkUtils::endSingleTimeCommands(mDevice, cmd);
}

void DeviceBuffer::update(vk::CommandBuffer& cmd, const void* data, const BufferInfo& info /*= {}*/) const
{
	const u64 targetStride = (info.mStride == VK_WHOLE_SIZE ? mBufferSize : info.mStride);

	ERM_ASSERT(targetStride + info.mOffset <= mBufferSize);

	if (targetStride >= 65536)
	{
		if (!mStagingBuffer || mStagingBuffer->getBufferSize() < targetStride)
		{
			mStagingBuffer.reset(new HostBuffer(mDevice, targetStride, BufferUsage::TRANSFER_SRC));
		}

		mStagingBuffer->update(data, {0, targetStride});

		vk::BufferCopy copyRegion {};
		copyRegion.size = targetStride;
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = info.mOffset;
		cmd.copyBuffer(mStagingBuffer->getBuffer(), mBuffer.get(), 1, &copyRegion);
	}
	else
	{
		cmd.updateBuffer(mBuffer.get(), info.mOffset, targetStride, data);
	}
}

} // namespace erm
