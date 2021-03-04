#pragma once

namespace erm::ecs {
	struct RenderingComponent;
}

namespace ImGui {

	extern bool ShowRenderingComponentDebug(erm::ecs::RenderingComponent& renderingComponent);

}