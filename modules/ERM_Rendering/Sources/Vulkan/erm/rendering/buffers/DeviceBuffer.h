#pragma once

#include "erm/rendering/buffers/IBuffer.h"

#include <erm/math/Types.h>

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
		u64 size,
		BufferUsageFlags buf);
	~DeviceBuffer();

	DeviceBuffer(DeviceBuffer&& other) noexcept;
	DeviceBuffer(const DeviceBuffer&) = delete;

	DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;
	DeviceBuffer& operator=(const DeviceBuffer&) = delete;

	void update(const void* data, const BufferInfo& info = {}) const override;
	void update(vk::CommandBuffer& cmd, const void* data, const BufferInfo& info = {}) const;

private:
	mutable std::unique_ptr<HostBuffer> mStagingBuffer;
};

} // namespace erm
