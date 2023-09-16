#pragma once

#include "erm/rendering/buffers/VertexArray.h"

#include <cstdint>

namespace erm {
class VertexBufferLayout;
} // namespace erm

namespace erm {

class VertexBuffer
{
public:
	VertexBuffer(VertexBufferLayout&& layout, const void* data, uint64_t size);
	~VertexBuffer();

	VertexBuffer(VertexBuffer&&) = delete;
	VertexBuffer(const VertexBuffer&) = delete;

	VertexBuffer& operator=(VertexBuffer&&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	void Bind() const;
	void Unbind() const;

	void BindVA() const;
	void UnbindVA() const;

private:
	unsigned int mRendererId;
	const VertexArray mVertexArray;
};

} // namespace erm
