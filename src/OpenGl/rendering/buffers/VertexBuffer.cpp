#include "erm/rendering/buffers/VertexBuffer.h"

#include "erm/utils/GlMacros.h"

#include <GL/glew.h>

namespace erm {
	
	VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	{
		GL_CALL(glGenBuffers(1, &mRendererId));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mRendererId));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
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
	
}
