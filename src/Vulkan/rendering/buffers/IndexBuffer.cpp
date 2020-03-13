#include "erm/rendering/buffers/IndexBuffer.h"

#include "erm/utils/Utils.h"

namespace erm {
	
	IndexBuffer::IndexBuffer(const IndexData* /*data*/, unsigned int count)
		: mCount(count)
	{
		UNUSED(mRendererId);
	}
	
	IndexBuffer::~IndexBuffer()
	{}
	
	void IndexBuffer::Bind() const
	{}
	
	void IndexBuffer::Unbind() const
	{}
	
}
