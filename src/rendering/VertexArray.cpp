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
	
	void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vbl)
	{
		Bind();
		vb.Bind();
		unsigned long offset = 0;
		const auto& elements = vbl.GetElements();
		
		for (unsigned int i = 0; i < elements.size(); ++i)
		{
			const auto& element = elements[i];
			GLCALL(glEnableVertexAttribArray(i));
			GLCALL(glVertexAttribPointer(i, element.mCount, element.mType, element.mNormalized, vbl.GetStride(), (const void*)offset));
			offset += element.mCount * VertexBufferElement::GetSizeOfType(element.mType);
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
