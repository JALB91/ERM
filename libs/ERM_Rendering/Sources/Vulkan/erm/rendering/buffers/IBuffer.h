#pragma once

#include "erm/rendering/buffers/BufferLayout.h"
#include "erm/rendering/enums/BufferUsage.h"
#include "erm/rendering/enums/MemoryProperty.h"

#include <vulkan/vulkan.hpp>

namespace erm {
class Device;
}

namespace erm {

class IBuffer
{
public:
	IBuffer(
		Device& device,
		u64 size,
		BufferUsageFlags buf,
		MemoryPropertyFlags mpf);
	virtual ~IBuffer() = default;

	IBuffer(const IBuffer&) noexcept;
	IBuffer(IBuffer&& other) noexcept;

	IBuffer& operator=(const IBuffer&) = delete;
	IBuffer& operator=(IBuffer&&) noexcept;

	virtual void update(const void* data, const BufferInfo& info) const = 0;

	inline void setLayout(BufferLayout&& layout) { mLayout = std::move(layout); }

	inline u64 getBufferSize() const { return mBufferSize; }
	inline const BufferLayout& getLayout() const { return mLayout; }
	inline vk::Buffer getBuffer() const { return mBuffer.get(); }
	inline vk::DeviceMemory getBufferMemory() const { return mBufferMemory.get(); }

protected:
	Device& mDevice;
	u64 mBufferSize;
	BufferLayout mLayout;

	BufferUsageFlags mBuf;
	MemoryPropertyFlags mMpf;
	vk::UniqueBuffer mBuffer;
	vk::UniqueDeviceMemory mBufferMemory;
};

} // namespace erm
