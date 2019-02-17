#pragma once

#include "Renderer.h"

#include <vector>

namespace erm {
	
	struct VertexBufferElement
	{
		unsigned int mType;
		unsigned int mCount;
		unsigned char mNormalized;
		
		static unsigned int GetSizeOfType(unsigned int type);
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
