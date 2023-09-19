#pragma once

#include <cstddef>
#include <cstdint>

namespace erm {

class IndexBuffer
{
public:
	IndexBuffer(void* data, u64 size, uint64_t count);
	~IndexBuffer();

	IndexBuffer(IndexBuffer&&) = delete;
	IndexBuffer(const IndexBuffer&) = delete;

	IndexBuffer& operator=(IndexBuffer&&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	void Bind() const;
	void Unbind() const;

	inline uint64_t GetCount() const { return mCount; }

private:
	const uint64_t mCount;
	unsigned int mRendererId;
};

} // namespace erm
