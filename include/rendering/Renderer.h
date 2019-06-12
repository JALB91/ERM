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
		
		void AddToQueue(const Entity& entity);
		void ProcessQueue(const Entity& camera);
		
	private:
		void Draw(const Entity& entity, const Entity& camera) const;
		void Draw(const VertexArray& va,const IndexBuffer& ib) const;
		
		const RenderContext& mRenderContext;
		
		std::queue<std::reference_wrapper<const Entity>> mRenderQueue;
		std::unique_ptr<Mesh> mDebugMesh;
		ShaderProgram* mDebugShader;
		ShaderProgram* mModelShader;
		
	};
	
}
