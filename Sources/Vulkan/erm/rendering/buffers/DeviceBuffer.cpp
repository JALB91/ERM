#include "erm/rendering/buffers/DeviceBuffer.h"

#include "erm/rendering/buffers/HostBuffer.h"

namespace erm {

	void DeviceBuffer::Update(vk::CommandBuffer& cmd, void* data) const
	{
		if (mBufferSize >= 65536)
		{
			if (!mStagingBuffer)
				mStagingBuffer.reset(new HostBuffer(mDevice, mBufferSize, vk::BufferUsageFlagBits::eTransferSrc));

			mStagingBuffer->Update(data);

			vk::BufferCopy copyRegion {};
			copyRegion.size = mBufferSize;
			cmd.copyBuffer(mStagingBuffer->GetBuffer(), mBuffer.get(), 1, &copyRegion);
		}
		else
		{
			cmd.updateBuffer(mBuffer.get(), 0, mBufferSize, data);
		}
	}

} // namespace erm