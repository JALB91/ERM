#pragma once

#include "erm/rendering/renderer/IRenderer.h"

namespace erm {
	class Engine;
}

namespace erm {

	class RTRenderer : public IRenderer
	{
	public:
		RTRenderer(Engine& engine);
		~RTRenderer();

		void OnPreRender() override;
		void OnRender() override;
		void OnPostRender() override;

		void SubmitRenderData(RenderData& data) override;
	};

} // namespace erm
