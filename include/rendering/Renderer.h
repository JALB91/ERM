#pragma once

#include "ec/Entity.h"

#include "math/mat.h"

#include <queue>
#include <memory>
#include <functional>

namespace erm {

	class Mesh;
	class Entity;
	class VertexArray;
	class IndexBuffer;
	class ShaderProgram;
	class RenderContext;
	
	class Renderer
	{
	public:
		Renderer(const RenderContext& renderContext);
		~Renderer();
		
		void ProcessQueue(const math::mat4& viewProjectionMatrix);
		
		void AddToQueue(const Entity& entity);
		
	private:
		void Draw(
			const VertexArray& va,
			const IndexBuffer& ib,
			const ShaderProgram& shader,
			const math::mat4& mvp
		) const;
		void Draw(
			const Mesh& mesh,
			const ShaderProgram& shader,
			const math::mat4& mvp
		) const;
		void Draw(
			const Entity& entity,
			const math::mat4& viewProjectionMatrix
		) const;
		
		const RenderContext& mRenderContext;
		
		std::queue<std::reference_wrapper<const Entity>> mRenderQueue;
		std::unique_ptr<Mesh> mDebugMesh;
		std::unique_ptr<ShaderProgram> mDebugShader;
		
	};
	
}
