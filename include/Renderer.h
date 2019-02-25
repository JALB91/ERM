#pragma once

#include <glm/glm.hpp>

namespace erm {

	class Mesh;
	class Model;
	class VertexArray;
	class IndexBuffer;
	class ShaderProgram;
	
	class Renderer
	{
	public:
		Renderer(int width, int height);
		~Renderer() = default;
		
		void OnSizeChanged(int width, int height);
		
		void Clear() const;
		void Draw(const VertexArray& va, const IndexBuffer& ib, ShaderProgram& shader, const glm::mat4& model) const;
		void Draw(const Mesh& mesh, ShaderProgram& shader, const glm::mat4& parent = glm::mat4(1.0f)) const;
		void Draw(const Model& model, ShaderProgram& shader) const;
		
	private:
		glm::mat4 mProjection;
		glm::mat4 mView;
		
	};
	
}
