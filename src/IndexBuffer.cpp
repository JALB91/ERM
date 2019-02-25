#include "IndexBuffer.h"
#include "Utils.h"

#include <GLFW/glfw3.h>

namespace erm {
	
	IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
		: mCount(count)
	{
		ASSERT(sizeof(unsigned int) == sizeof(GLuint));
		
		GLCALL(glGenBuffers(1, &mRendererId));
		GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId));
		GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
	}
	
	IndexBuffer::~IndexBuffer()
	{
		GLCALL(glDeleteBuffers(1, &mRendererId));
	}
	
	void IndexBuffer::Bind() const
	{
		GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId));
	}
	
	void IndexBuffer::Unbind() const
	{
		GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
	
}
