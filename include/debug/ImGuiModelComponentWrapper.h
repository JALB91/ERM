#pragma once

namespace erm {
	namespace ecs {
		struct ModelComponent;
	}
}

namespace ImGui {
	
	extern void ShowPathOptions(erm::ecs::ModelComponent& modelComponent);
	extern void ShowModelComponentDebugWindow(erm::ecs::ModelComponent& modelComponent);
	
}
