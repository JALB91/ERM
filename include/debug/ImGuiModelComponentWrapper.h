#pragma once

namespace erm {
	class Game;
	namespace ecs {
		struct ModelComponent;
	}
}

namespace ImGui {
	
	extern bool ShowModelComponentDebugWindow(erm::Game& game, erm::ecs::ModelComponent& modelComponent);
	
}
