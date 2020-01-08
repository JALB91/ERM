#pragma once

namespace erm {
	namespace ecs {
		struct Entity;
	}
}

namespace ImGui {
	
	void ShowComponentDebugWindow(erm::ecs::Entity* entity);
	
}
