#include "erm/rendering/buffers/VertexArray.h"

#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/buffers/VertexBufferLayout.h"
#include "erm/rendering/renderer/Renderer.h"

#include "erm/utils/GlUtils.h"

#include <GL/glew.h>

namespace erm {

	VertexArray::VertexArray()
	{
		GL_CALL(glGenVertexArrays(1, &mRendererId));
	}

	VertexArray::~VertexArray()
	{
		GL_CALL(glDeleteVertexArrays(1, &mRendererId));
	}

	void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vbl) const
	{
		Bind();
		vb.Bind();
		size_t offset = 0;
		const std::vector<VertexBufferElement>& elements = vbl.GetElements();

		for (unsigned int i = 0; i < static_cast<unsigned int>(elements.size()); ++i)
		{
			const VertexBufferElement& element = elements[i];
			GL_CALL(glEnableVertexAttribArray(i));
			switch (element.mType)
			{
				case GL_FLOAT:
				case GL_DOUBLE:
					GL_CALL(glVertexAttribPointer(i, element.mCount, element.mType, element.mNormalized, vbl.GetStride(), reinterpret_cast<const void*>(offset)));
					break;
				case GL_UNSIGNED_INT:
				case GL_UNSIGNED_BYTE:
					GL_CALL(glVertexAttribIPointer(i, element.mCount, element.mType, vbl.GetStride(), reinterpret_cast<const void*>(offset)));
					break;
				default:
					ASSERT(false);
					break;
			}

			offset += element.mCount * element.mOffset;
		}
	}

	void VertexArray::Bind() const
	{
		GL_CALL(glBindVertexArray(mRendererId));
	}

	void VertexArray::Unbind() const
	{
		GL_CALL(glBindVertexArray(0));
	}

} // namespace erm
