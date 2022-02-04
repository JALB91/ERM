#pragma once

namespace erm {
class Engine;
struct RenderData;
}

namespace erm {

class Renderer
{
public:
	Renderer(Engine& engine);

	void OnPreRender();
	void OnRender();
	void OnPostRender();

	void SubmitRenderData(RenderData& data);

private:
	Engine& mEngine;
};

} // namespace erm