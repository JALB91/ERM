#pragma once

#include <vulkan/vulkan.hpp>

#include <memory>

namespace erm {
	class Device;
}

namespace erm {

	class IndexBuffer
	{
	public:
		IndexBuffer(Device& device, void* data, size_t size, uint32_t count);
		~IndexBuffer();

		IndexBuffer(IndexBuffer&&) = delete;
		IndexBuffer(const IndexBuffer&) = delete;

		IndexBuffer& operator=(IndexBuffer&&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		void Bind(const vk::CommandBuffer& commandBuffer) const;

		inline uint32_t GetCount() const { return mCount; }

	private:
		Device& mDevice;
		const uint32_t mCount;

		vk::Buffer mBuffer;
		vk::DeviceMemory mBufferMemory;
	};

} // namespace erm
