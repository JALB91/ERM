#pragma once

#include <glm/glm.hpp>

#include <memory>

namespace erm {

	class Game;
	class Mesh;
	class Entity;
	class VertexArray;
	class IndexBuffer;
	class ShaderProgram;
	
	class Renderer
	{
	public:
		Renderer(const Game& game);
		~Renderer();
		
		void UpdateProjection();
		void OnPreRender();
		
		void Clear() const;
		void Draw(const VertexArray& va, const IndexBuffer& ib, const ShaderProgram& shader, const glm::mat4& model) const;
		void Draw(const Mesh& mesh, const ShaderProgram& shader, const glm::mat4& transform = glm::mat4(1.0f)) const;
		void Draw(const Entity& entity) const;
		
	private:
		const Game& mGame;
		glm::mat4 mProjection;
		glm::mat4 mView;
		glm::mat4 mViewProjection;
		
		std::unique_ptr<Mesh> mDebugMesh;
		std::unique_ptr<ShaderProgram> mDebugShader;
		
	};
	
}
