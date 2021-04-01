#pragma once

#include "erm/rendering/buffers/BufferLayout.h"

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
		vk::BufferUsageFlags buf,
		vk::MemoryPropertyFlags mpf);
	virtual ~IBuffer() = default;

	IBuffer(const IBuffer&) = delete;
	IBuffer(IBuffer&& other);

	IBuffer& operator=(const IBuffer&) = delete;
	IBuffer& operator=(IBuffer&&);

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

	vk::UniqueBuffer mBuffer;
	vk::UniqueDeviceMemory mBufferMemory;
};

} // namespace erm
