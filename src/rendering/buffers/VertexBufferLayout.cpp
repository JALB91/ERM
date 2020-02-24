#include "rendering/buffers/VertexBufferLayout.h"
#include "utils/Utils.h"

#include <GL/glew.h>

namespace erm {
	
	template<typename T>
	void VertexBufferLayout::Push(unsigned int /*count*/)
	{
		ASSERT(false);
	}

	template<>
	void VertexBufferLayout::Push<float>(unsigned int count)
	{
		static const unsigned int offset = sizeof(float);
		mElements.push_back({ GL_FLOAT, count, offset, GL_FALSE });
		mStride += count * offset;
	}
	
	template<>
	void VertexBufferLayout::Push<double>(unsigned int count)
	{
		static const unsigned int offset = sizeof(double);
		mElements.push_back({ GL_DOUBLE, count, offset, GL_FALSE });
		mStride += count * offset;
	}

	template<>
	void VertexBufferLayout::Push<unsigned int>(unsigned int count)
	{
		static const unsigned int offset = sizeof(unsigned int);
		mElements.push_back({ GL_UNSIGNED_INT, count, offset, GL_FALSE });
		mStride += count * offset;
	}

	template<>
	void VertexBufferLayout::Push<unsigned char>(unsigned int count)
	{
		static const unsigned int offset = sizeof(unsigned char);
		mElements.push_back({ GL_UNSIGNED_BYTE, count, offset, GL_TRUE });
		mStride += count * offset;
	}
	
}
