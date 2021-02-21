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
		virtual ~IBuffer();

		IBuffer(const IBuffer&) = delete;
		IBuffer(IBuffer&& other);

		IBuffer& operator=(const IBuffer&) = delete;
		IBuffer& operator=(IBuffer&&) = delete;

		inline vk::Buffer GetBuffer() const { return mBuffer; }
		inline size_t GetBufferSize() const { return mBufferSize; }

	protected:
		Device& mDevice;
		const size_t mBufferSize;

		vk::Buffer mBuffer;
		vk::DeviceMemory mBufferMemory;
	};

} // namespace erm
