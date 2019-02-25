#include "Renderer.h"
#include "Mesh.h"
#include "Model.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ShaderProgram.h"
#include "Uniforms.h"
#include "Utils.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <iostream>

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
	
	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const ShaderProgram& shader, const glm::mat4& model) const
	{
		const glm::mat4 vp = mProjection * mView;
		
		va.Bind();
		ib.Bind();
		shader.Bind();
		shader.SetUniformMat4f(Uniform::MVP, vp * model);
		GLCALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	}
	
	void Renderer::Draw(const Mesh& mesh, const ShaderProgram& shader, const glm::mat4& parent /* = glm::mat4(1.0f) */) const
	{
		glm::mat4 model = glm::translate(parent, mesh.GetTranslation());
		model = glm::rotate(model, mesh.GetRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, mesh.GetRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, mesh.GetRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
		Draw(mesh.GetVA(), mesh.GetIB(), shader, model);
	}
	
	void Renderer::Draw(const Model& model, const ShaderProgram& shader) const
	{
		glm::mat4 parent = glm::translate(glm::mat4(1.0f), model.GetTranslation());
		parent = glm::rotate(parent, model.GetRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
		parent = glm::rotate(parent, model.GetRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
		parent = glm::rotate(parent, model.GetRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
		
		for (const Mesh& mesh: model.GetMeshes())
		{
			Draw(mesh, shader, parent);
		}
	}
	
}
