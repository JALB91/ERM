#pragma once

namespace erm {
	class Engine;
	namespace ecs {
		struct ModelComponent;
	}
}

namespace ImGui {
	
	extern bool ShowModelComponentDebugWindow(erm::Engine& engine, erm::ecs::ModelComponent& modelComponent);
	
}
