#pragma once

namespace erm {
	
	class Game;
	
	namespace ecs {
		struct SkeletonComponent;
	}
}

namespace ImGui {
	
	extern bool ShowSkeletonComponentDebugWindow(erm::Game& game, erm::ecs::SkeletonComponent& skeletonComponent);
	
}
