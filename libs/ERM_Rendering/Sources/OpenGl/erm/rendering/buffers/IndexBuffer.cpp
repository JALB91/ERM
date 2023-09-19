#include "erm/rendering/buffers/IndexBuffer.h"

#include "erm/utils/GlUtils.h"

#include <GL/glew.h>

namespace erm {

IndexBuffer::IndexBuffer(void* data, u64 size, uint64_t count)
	: mCount(count)
{
	ASSERT((size / count) == sizeof(GLuint));

	GL_CALL(glGenBuffers(1, &mRendererId));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GL_CALL(glDeleteBuffers(1, &mRendererId));
}

void IndexBuffer::Bind() const
{
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId));
}

void IndexBuffer::Unbind() const
{
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

} // namespace erm
