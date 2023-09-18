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
		size_t size,
		BufferUsageFlags buf,
		MemoryPropertyFlags mpf);
	virtual ~IBuffer() = default;

	IBuffer(const IBuffer&) noexcept;
	IBuffer(IBuffer&& other) noexcept;

	IBuffer& operator=(const IBuffer&) = delete;
	IBuffer& operator=(IBuffer&&) noexcept;

	virtual void Update(const void* data, const BufferInfo& info) const = 0;

	inline void SetLayout(BufferLayout&& layout) { mLayout = std::move(layout); }

	inline size_t GetBufferSize() const { return mBufferSize; }
	inline const BufferLayout& GetLayout() const { return mLayout; }
	inline vk::Buffer GetBuffer() const { return mBuffer.get(); }
	inline vk::DeviceMemory GetBufferMemory() const { return mBufferMemory.get(); }

protected:
	Device& mDevice;
	size_t mBufferSize;
	BufferLayout mLayout;

	BufferUsageFlags mBuf;
	MemoryPropertyFlags mMpf;
	vk::UniqueBuffer mBuffer;
	vk::UniqueDeviceMemory mBufferMemory;
};

} // namespace erm
