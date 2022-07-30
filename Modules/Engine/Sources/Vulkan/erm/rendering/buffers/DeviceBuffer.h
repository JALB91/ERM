#pragma once

#include "erm/rendering/buffers/IBuffer.h"

#include <memory>

namespace erm {
class HostBuffer;
}

namespace erm {

class DeviceBuffer : public IBuffer
{
public:
	DeviceBuffer(
		Device& device,
		size_t size,
		BufferUsageFlags buf);
	~DeviceBuffer();

	DeviceBuffer(DeviceBuffer&& other);
	DeviceBuffer(const DeviceBuffer&) = delete;

	DeviceBuffer& operator=(DeviceBuffer&& other);
	DeviceBuffer& operator=(const DeviceBuffer&) = delete;

	void Update(const void* data, const BufferInfo& info = {}) const override;
	void Update(vk::CommandBuffer& cmd, const void* data, const BufferInfo& info = {}) const;

private:
	mutable std::unique_ptr<HostBuffer> mStagingBuffer;
};

} // namespace erm
