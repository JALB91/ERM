#include "erm/rendering/buffers/VertexArray.h"

#include "erm/utils/Utils.h"

namespace erm {
	
	VertexArray::VertexArray()
	{
		UNUSED(mRendererId);
	}
	
	VertexArray::~VertexArray()
	{}
	
	void VertexArray::AddBuffer(const VertexBuffer& /*vb*/, const VertexBufferLayout& /*vbl*/) const
	{}
	
	void VertexArray::Bind() const
	{}
	
	void VertexArray::Unbind() const
	{}
	
}
