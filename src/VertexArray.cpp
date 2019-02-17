#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>

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
		const auto& elements = vbl.GetElements();
		unsigned long offset = 0;
		
		for (int i = 0; i < elements.size(); ++i)
		{
			const auto& element = elements[i];
			GLCALL(glEnableVertexAttribArray(i));
			GLCALL(glVertexAttribPointer(i, element.mCount, element.mType,
				element.mNormalized, vbl.GetStride(), (const void*)offset));
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
