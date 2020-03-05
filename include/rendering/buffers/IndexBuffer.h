#pragma once

#include "rendering/buffers/IndexData.h"

namespace erm {

	class IndexBuffer
	{
	public:
		IndexBuffer(const IndexData* data, unsigned int count);
		~IndexBuffer();
		
		IndexBuffer(IndexBuffer&&) = delete;
		IndexBuffer(const IndexBuffer&) = delete;
		
		IndexBuffer& operator=(IndexBuffer&&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;
		
		void Bind() const;
		void Unbind() const;
		
		inline unsigned int GetCount() const { return mCount; }
		
	private:
		const unsigned int mCount;
		unsigned int mRendererId;
		
	};
	
}
