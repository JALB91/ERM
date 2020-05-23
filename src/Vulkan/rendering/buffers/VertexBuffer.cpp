#include "erm/rendering/buffers/VertexBuffer.h"

#include "erm/utils/Utils.h"

namespace erm {

	VertexBuffer::VertexBuffer(const void* /*data*/, unsigned int /*size*/)
	{
		UNUSED(mRendererId);
	}

	VertexBuffer::~VertexBuffer()
	{}

	void VertexBuffer::Bind() const
	{}

	void VertexBuffer::Unbind() const
	{}

} // namespace erm
