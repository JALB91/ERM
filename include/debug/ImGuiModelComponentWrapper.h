#pragma once

namespace erm {
	namespace ecs {
		struct ModelComponent;
	}
}

namespace ImGui {
	
	extern void ShowPathOptions(erm::ecs::ModelComponent& modelComponent);
	extern bool ShowModelComponentDebugWindow(erm::ecs::ModelComponent& modelComponent);
	
}
