#include "Renderer.h"
#include "Mesh.h"
#include "Model.h"
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
		: mProjection(glm::perspective(glm::radians(45.0f), static_cast<float>(width)/static_cast<float>(height), 0.1f, 100.0f))
		, mView(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -10.0f)))
	{}
	
	void Renderer::OnSizeChanged(int width, int height)
	{
		mProjection = glm::perspective(glm::radians(45.0f), static_cast<float>(width)/static_cast<float>(height), 0.1f, 100.0f);
	}
	
	void Renderer::Clear() const
	{
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	
	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, ShaderProgram& shader, const glm::mat4& model) const
	{
		const glm::mat4 vp = mProjection * mView;
		
		va.Bind();
		ib.Bind();
		shader.Bind();
		shader.SetUniformMat4f("u_MVP", vp * model);
		GLCALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	}
	
	void Renderer::Draw(const Mesh& mesh, ShaderProgram& shader, const glm::mat4& parent /* = glm::mat4(1.0f) */) const
	{
		glm::mat4 model = glm::translate(parent, mesh.GetTranslation());
		model = glm::rotate(model, mesh.GetRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, mesh.GetRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, mesh.GetRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
		Draw(mesh.GetVA(), mesh.GetIB(), shader, model);
	}
	
	void Renderer::Draw(const Model& model, ShaderProgram& shader) const
	{
		
	}
	
}
