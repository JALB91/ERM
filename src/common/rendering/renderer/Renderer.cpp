#include "erm/rendering/renderer/Renderer.h"

#include "erm/rendering/buffers/VertexArray.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/renderer/RenderContext.h"

#include <GL\glew.h>

namespace erm {
	
	Renderer::Renderer(const RenderContext& renderContext)
		: mRenderContext(renderContext)
	{
		mRenderContext.SetDepthEnabled(true);
		mRenderContext.SetDepthFunction(GL_LESS);

		mRenderContext.SetBlendEnabled(true);
		mRenderContext.SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mRenderContext.SetCullFaceEnabled(true);
		mRenderContext.SetCullFace(GL_FRONT);
#if defined(GLM_FORCE_LEFT_HANDED)
		mRenderContext.SetFrontFace(GL_CCW);
#else
		mRenderContext.SetFrontFace(GL_CW);
#endif

		mRenderContext.SetClearColor(math::vec4(0.25f, 0.25f, 0.25f, 1.0f));
	}
	
	void Renderer::Draw(DrawMode drawMode, const VertexArray& va, const IndexBuffer& ib) const
	{
		va.Bind();
		ib.Bind();
		mRenderContext.Draw(drawMode, ib.GetCount());
	}
	
}
