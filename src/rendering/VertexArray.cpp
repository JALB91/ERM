#include "rendering/VertexArray.h"
#include "rendering/VertexBuffer.h"
#include "rendering/VertexBufferLayout.h"
#include "rendering/Renderer.h"

#include "utils/Utils.h"

#include <GL/glew.h>

namespace erm {
	
	VertexArray::VertexArray()
	{
		GLCALL(glGenVertexArrays(1, &mRendererId));
	}
	
	VertexArray::~VertexArray()
	{
		GLCALL(glDeleteVertexArrays(1, &mRendererId));
	}
	
	void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vbl) const
	{
		Bind();
		vb.Bind();
		unsigned long offset = 0;
		const std::vector<VertexBufferElement>& elements = vbl.GetElements();
		
		for (unsigned int i = 0; i < elements.size(); ++i)
		{
			const VertexBufferElement& element = elements[i];
			GLCALL(glEnableVertexAttribArray(i));
			switch (element.mType)
			{
				case GL_FLOAT:
				case GL_DOUBLE:
				case GL_UNSIGNED_INT:
				case GL_UNSIGNED_BYTE:
					GLCALL(glVertexAttribPointer(i, element.mCount, element.mType, element.mNormalized, vbl.GetStride(), (const void*)offset));
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
		GLCALL(glBindVertexArray(mRendererId));
	}
	
	void VertexArray::Unbind() const
	{
		GLCALL(glBindVertexArray(0));
	}
	
}
