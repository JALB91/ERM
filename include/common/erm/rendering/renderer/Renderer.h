#pragma once

#include "erm/rendering/enums/DrawMode.h"

namespace erm {
	class VertexArray;
	class IndexBuffer;
	class RenderContext;
}

namespace erm {
	
	class Renderer
	{
	public:
		Renderer(const RenderContext& renderContext);
		
		inline const RenderContext& GetRenderContext() const { return mRenderContext; }
		
		void Draw(DrawMode drawMode, const VertexArray& va, const IndexBuffer& ib) const;
		
	private:
		const RenderContext& mRenderContext;
		
	};
	
}
