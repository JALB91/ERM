#pragma once

#include <vulkan/vulkan.hpp>

namespace erm {
	class Device;
}

namespace erm {

	class Buffer
	{
	public:
		Buffer(Device& device, size_t size);
		virtual ~Buffer();

		Buffer(const Buffer&) = delete;
		Buffer(Buffer&& other);

		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&&) = delete;

		inline vk::Buffer GetBuffer() const { return mBuffer; }
		inline size_t GetBufferSize() const { return mBufferSize; }

	protected:
		Device& mDevice;
		const size_t mBufferSize;

		vk::Buffer mBuffer;
		vk::DeviceMemory mBufferMemory;
	};

} // namespace erm
