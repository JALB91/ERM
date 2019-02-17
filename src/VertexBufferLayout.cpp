#include "VertexBufferLayout.h"

#include <GLFW/glfw3.h>

namespace erm {
	
	unsigned int VertexBufferElement::GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT: 			return 4;
			case GL_UNSIGNED_INT: 	return 4;
			case GL_UNSIGNED_BYTE: 	return 1;
		}
		ASSERT(false);
		return 0;
	}
	
	template<>
	void VertexBufferLayout::Push<float>(unsigned int count)
	{
		mElements.push_back({ GL_FLOAT, count, GL_FALSE });
		mStride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	
	template<>
	void VertexBufferLayout::Push<unsigned int>(unsigned int count)
	{
		mElements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		mStride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}
	
	template<>
	void VertexBufferLayout::Push<unsigned char>(unsigned int count)
	{
		mElements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		mStride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}
	
}
