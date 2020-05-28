#pragma once

#include <vulkan/vulkan.hpp>

#include <memory>

namespace erm {
	class Device;
}

namespace erm {

	class VertexBuffer
	{
	public:
		VertexBuffer(Device& device, void* data, size_t size);
		~VertexBuffer();

		VertexBuffer(VertexBuffer&&) = delete;
		VertexBuffer(const VertexBuffer&) = delete;

		VertexBuffer& operator=(VertexBuffer&&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		void Bind(const vk::CommandBuffer& commandBuffer) const;

	private:
		Device& mDevice;
		vk::Buffer mBuffer;
		vk::DeviceMemory mBufferMemory;
	};

} // namespace erm
