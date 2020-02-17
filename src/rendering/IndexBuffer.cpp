#include "rendering/IndexBuffer.h"
#include "utils/Utils.h"

#include <GL/glew.h>

namespace erm {
	
	IndexBuffer::IndexBuffer(const unsigned short* data, unsigned int count)
		: mCount(count)
	{
		ASSERT(sizeof(unsigned short) == sizeof(GLushort));
		
		GL_CALL(glGenBuffers(1, &mRendererId));
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId));
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned short), data, GL_STATIC_DRAW));
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
	
}
