#pragma once

namespace erm {
	namespace ecs {
		struct ModelComponent;
	}
}

namespace ImGui {
	
	void ShowPathOptions(erm::ecs::ModelComponent& modelComponent);
	void ShowModelComponentDebugWindow(erm::ecs::ModelComponent& modelComponent);
	
}
