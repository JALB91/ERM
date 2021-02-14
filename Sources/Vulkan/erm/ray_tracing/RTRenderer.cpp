#include "erm/ray_tracing/RTRenderer.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/data_structs/RenderData.h"

namespace erm {

	RTRenderer::RTRenderer(Engine& engine)
		: IRenderer(engine)
	{}

	RTRenderer::~RTRenderer()
	{}

	void RTRenderer::OnPreRender()
	{
	}

	void RTRenderer::OnRender()
	{
	}

	void RTRenderer::OnPostRender()
	{
	}

	void RTRenderer::SubmitRenderData(RenderData& data)
	{
	}

} // namespace erm
