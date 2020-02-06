#pragma once

namespace erm {
	namespace ecs {
		struct EntityId;
	}
}

namespace ImGui {
	
	extern void ShowComponentDebugWindow(erm::ecs::EntityId entity);
	
}
