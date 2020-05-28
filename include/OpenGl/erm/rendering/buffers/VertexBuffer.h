#pragma once

#include <memory>

namespace erm {
	class IDevice;
	class VertexBufferLayout;
} // namespace erm

namespace erm {

	class VertexBuffer
	{
	public:
		VertexBuffer(IDevice& device, VertexBufferLayout&& layout, void* data, uint64_t size);
		~VertexBuffer();

		VertexBuffer(VertexBuffer&&) = delete;
		VertexBuffer(const VertexBuffer&) = delete;

		VertexBuffer& operator=(VertexBuffer&&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		template<typename T>
		void Bind(T arg) const;
		void Unbind() const;

	private:
		class Impl;
		std::unique_ptr<Impl> mImpl;
	};

} // namespace erm
