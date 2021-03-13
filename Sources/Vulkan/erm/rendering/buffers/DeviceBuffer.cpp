#include "erm/rendering/buffers/DeviceBuffer.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/HostBuffer.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

	DeviceBuffer::DeviceBuffer(DeviceBuffer&& other)
		: IBuffer(std::move(other))
		, mStagingBuffer(std::move(other.mStagingBuffer))
	{}

	DeviceBuffer& DeviceBuffer::operator=(DeviceBuffer&& other)
	{
		if (this == &other)
			return *this;

		IBuffer::operator=(std::move(other));
		mStagingBuffer = std::move(other.mStagingBuffer);

		return *this;
	}

	void DeviceBuffer::Update(const void* data, const BufferInfo& info /*= {}*/) const
	{
		vk::CommandBuffer cmd = VkUtils::BeginSingleTimeCommands(mDevice);
		Update(cmd, data, info);
		VkUtils::EndSingleTimeCommands(mDevice, cmd);
	}

	void DeviceBuffer::Update(vk::CommandBuffer& cmd, const void* data, const BufferInfo& info /*= {}*/) const
	{
		const size_t targetStride = info.mStride == VK_WHOLE_SIZE ? mBufferSize : info.mStride;

		ASSERT(targetStride + info.mOffset <= mBufferSize);

		if (targetStride >= 65536)
		{
			if (!mStagingBuffer || mStagingBuffer->GetBufferSize() < targetStride)
				mStagingBuffer.reset(new HostBuffer(mDevice, targetStride, vk::BufferUsageFlagBits::eTransferSrc));

			mStagingBuffer->Update(data, {0, targetStride});

			vk::BufferCopy copyRegion {};
			copyRegion.size = targetStride;
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = info.mOffset;
			cmd.copyBuffer(mStagingBuffer->GetBuffer(), mBuffer.get(), 1, &copyRegion);
		}
		else
		{
			cmd.updateBuffer(mBuffer.get(), info.mOffset, targetStride, data);
		}
	}

} // namespace erm