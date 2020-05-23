#pragma once

namespace erm {
	class Engine;
	namespace ecs {
		struct ModelComponent;
	}
} // namespace erm

namespace ImGui {

	extern bool ShowModelComponentDebugWindow(erm::Engine& engine, erm::ecs::ModelComponent& modelComponent);

}
