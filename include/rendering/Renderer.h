#pragma once

#include "interfaces/IWindowSizeListener.h"

#include <glm/glm.hpp>

#include <memory>

namespace erm {

	class Mesh;
	class Entity;
	class VertexArray;
	class IndexBuffer;
	class ShaderProgram;
	class RenderContext;
	class IWindowSizeProvider;
	
	class Renderer : private IWindowSizeListener
	{
	public:
		Renderer(
			const RenderContext& renderContext,
			IWindowSizeProvider& windowSizeProvider
		);
		~Renderer();
		
		void UpdateProjection();
		void OnPreRender();
		
		void Draw(const VertexArray& va, const IndexBuffer& ib, const ShaderProgram& shader, const glm::mat4& model) const;
		void Draw(const Mesh& mesh, const ShaderProgram& shader, const glm::mat4& transform = glm::mat4(1.0f)) const;
		void Draw(const Entity& entity) const;
		
	private:
		// IWindowSizeListener
		void OnSizeChanged(int width, int height) override;
		
		const RenderContext& mRenderContext;
		IWindowSizeProvider& mWindowSizeProvider;
		
		glm::mat4 mProjection;
		glm::mat4 mView;
		glm::mat4 mViewProjection;
		
		std::unique_ptr<Mesh> mDebugMesh;
		std::unique_ptr<ShaderProgram> mDebugShader;
		
	};
	
}
