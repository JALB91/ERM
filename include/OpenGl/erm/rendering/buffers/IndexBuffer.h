#pragma once

#include "erm/rendering/buffers/IndexData.h"

#include <memory>

namespace erm {
	class IDevice;
}

namespace erm {

	class IndexBuffer
	{
	public:
		IndexBuffer(IDevice& device, IndexData* data, uint64_t count);
		~IndexBuffer();

		IndexBuffer(IndexBuffer&&) = delete;
		IndexBuffer(const IndexBuffer&) = delete;

		IndexBuffer& operator=(IndexBuffer&&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		template<typename T>
		void Bind(T arg) const;
		void Unbind() const;

		uint64_t GetCount() const;

	private:
		class Impl;
		std::unique_ptr<Impl> mImpl;
	};

} // namespace erm
