#include "erm/rendering/renderer/Renderer.h"

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/renderer/RenderContext.h"

#include "erm/utils/Utils.h"

#include <GL/glew.h>

#include <iostream>

namespace erm {

Renderer::Renderer(Engine& engine)
	: mEngine(engine)
{
	UNUSED(mEngine);

	mRenderContext.SetDepthEnabled(true);
	mRenderContext.SetDepthFunction(DepthFunction::LESS);

	mRenderContext.SetBlendEnabled(true);
	mRenderContext.SetBlendFunction(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);

	mRenderContext.SetCullFaceEnabled(true);
	mRenderContext.SetCullMode(CullMode::FRONT);
#if defined(GLM_FORCE_LEFT_HANDED)
	mRenderContext.SetFrontFace(FrontFace::CCW);
#else
	mRenderContext.SetFrontFace(FrontFace::CW);
#endif

	mRenderContext.SetClearColor(math::vec4(0.25f, 0.25f, 0.25f, 1.0f));

	std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void Renderer::OnPreRender()
{
	mRenderContext.Clear();
}

void Renderer::OnRender()
{}

void Renderer::OnPostRender()
{}

void Renderer::Draw(DrawMode drawMode, const VertexBuffer& vb, const IndexBuffer& ib) const
{
	vb.BindVA();
	ib.Bind();
	mRenderContext.Draw(drawMode, ib.GetCount());
}

} // namespace erm
