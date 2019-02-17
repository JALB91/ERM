#include "VertexBuffer.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>

namespace erm {
	
	VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	{
		GLCALL(glGenBuffers(1, &mRendererId));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, mRendererId));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	}
	
	VertexBuffer::~VertexBuffer()
	{
		GLCALL(glDeleteBuffers(1, &mRendererId));
	}
	
	void VertexBuffer::Bind() const
	{
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, mRendererId));
	}
	
	void VertexBuffer::Unbind() const
	{
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
	
}
