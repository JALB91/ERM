#pragma once

#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/renderer/RenderContext.h"

namespace erm {
	class Engine;
	class VertexBuffer;
	class IndexBuffer;
} // namespace erm

namespace erm {

	class Renderer
	{
	public:
		Renderer(Engine& engine);

		inline const RenderContext& GetRenderContext() const { return mRenderContext; }

		void OnPreRender();
		void OnRender();
		void OnPostRender();

		void Draw(DrawMode drawMode, const VertexBuffer& vb, const IndexBuffer& ib) const;

	private:
		Engine& mEngine;
		RenderContext mRenderContext;
	};

} // namespace erm
