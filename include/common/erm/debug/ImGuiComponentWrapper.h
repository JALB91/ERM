#pragma once

namespace erm {
	class Engine;
	namespace ecs {
		struct EntityId;
	}
}

namespace ImGui {
	
	extern void ShowComponentDebugWindow(erm::Engine& engine, erm::ecs::EntityId entity);
	
}
