#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ShaderProgram.h"

#include <glm/gtc/matrix_transform.hpp>

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
	
	Renderer::Renderer(int width, int height)
		: mProjection(glm::perspective(1000.0f, static_cast<float>(width)/static_cast<float>(height), 0.0f, 100.0f))
		, mView(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -500.0f)))
	{}
	
	void Renderer::Clear() const
	{
		GLCALL(glClear(GL_COLOR_BUFFER_BIT));
	}
	
	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, ShaderProgram& shader, const glm::mat4& model) const
	{
		const glm::mat4 vm = mProjection * mView;
		
		va.Bind();
		ib.Bind();
		shader.Bind();
		shader.SetUniformMat4f("u_MVP", vm * glm::inverse(model));
		GLCALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	}
	
}
