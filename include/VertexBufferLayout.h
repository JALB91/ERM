#pragma once

#include "Renderer.h"

#include <GLFW/glfw3.h>

#include <vector>

namespace erm {
	
	struct VertexBufferElement
	{
		unsigned int mType;
		unsigned int mCount;
		unsigned char mNormalized;
		
		static unsigned int GetSizeOfType(unsigned int type)
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
	};
	
	class VertexBufferLayout
	{
	public:
		VertexBufferLayout()
			: mStride(0)
		{}
		
		template<typename T>
		inline void Push(unsigned int count)
		{
			ASSERT(false);
		}
		
		inline const std::vector<VertexBufferElement>& GetElements() const { return mElements; }
		inline unsigned int GetStride() const { return mStride; }
		
	private:
		std::vector<VertexBufferElement> mElements;
		unsigned int mStride;
		
	};
	
}
