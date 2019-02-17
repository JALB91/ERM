#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ShaderProgram.h"

#include <GLFW/glfw3.h>

#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL error] (" << error << ") " << function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

namespace erm {
	
	void Renderer::Clear() const
	{
		GLCALL(glClear(GL_COLOR_BUFFER_BIT));
	}
	
	void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const ShaderProgram &shader) const
	{
		va.Bind();
		ib.Bind();
		shader.Bind();
		GLCALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	}
	
}
