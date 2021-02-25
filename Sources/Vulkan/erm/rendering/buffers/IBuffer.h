#pragma once

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
		IBuffer& operator=(IBuffer&&) = delete;

		inline size_t GetBufferSize() const { return mBufferSize; }
		inline vk::Buffer GetBuffer() const { return mBuffer.get(); }
		inline vk::DeviceMemory GetBufferMemory() const { return mBufferMemory.get(); }

	protected:
		Device& mDevice;
		const size_t mBufferSize;

		vk::UniqueBuffer mBuffer;
		vk::UniqueDeviceMemory mBufferMemory;
	};

} // namespace erm
