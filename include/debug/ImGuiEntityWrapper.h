#pragma once

namespace erm {
	namespace ecs {
		struct Entity;
	}
}

namespace ImGui {
	
	void ShowEntityDebugWindow(erm::ecs::Entity** active, erm::ecs::Entity& entity);
	
}
