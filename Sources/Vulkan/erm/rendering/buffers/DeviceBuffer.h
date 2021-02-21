#pragma once

#include "erm/rendering/buffers/HostBuffer.h"
#include "erm/rendering/buffers/IBuffer.h"

#include <memory>

namespace erm {

	class DeviceBuffer : public IBuffer
	{
	public:
		DeviceBuffer(
			Device& device,
			size_t size,
			vk::BufferUsageFlags buf)
			: IBuffer(device, size, buf | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal)
		{}

		void Update(vk::CommandBuffer& cmd, void* data) const;

	private:
		mutable std::unique_ptr<HostBuffer> mStagingBuffer;
	};

} // namespace erm
