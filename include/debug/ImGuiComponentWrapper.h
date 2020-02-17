#pragma once

namespace erm {
	class Game;
	namespace ecs {
		struct EntityId;
	}
}

namespace ImGui {
	
	extern void ShowComponentDebugWindow(erm::Game& game, erm::ecs::EntityId entity);
	
}
