#include "erm/rendering/buffers/VertexBuffer.h"

#include "erm/utils/GlUtils.h"

#include <GL/glew.h>

namespace erm {

VertexBuffer::VertexBuffer(VertexBufferLayout&& layout, const void* data, uint64_t size)
{
	GL_CALL(glGenBuffers(1, &mRendererId));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mRendererId));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

	mVertexArray.AddBuffer(*this, layout);
}

VertexBuffer::~VertexBuffer()
{
	GL_CALL(glDeleteBuffers(1, &mRendererId));
}

void VertexBuffer::Bind() const
{
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mRendererId));
}

void VertexBuffer::Unbind() const
{
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::BindVA() const
{
	mVertexArray.Bind();
}

void VertexBuffer::UnbindVA() const
{
	mVertexArray.Unbind();
}

} // namespace erm
