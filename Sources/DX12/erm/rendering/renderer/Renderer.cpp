#include "erm/rendering/renderer/Renderer.h"

namespace erm {

Renderer::Renderer(Engine& engine)
	: mEngine(engine)
{}

void Renderer::OnPreRender()
{
}

void Renderer::OnRender()
{
}

void Renderer::OnPostRender()
{
}

void Renderer::SubmitRenderData(RenderData& data)
{}

} // namespace erm