#pragma once

namespace erm {
	
	class Game;
	
	namespace ecs {
		struct AnimationComponent;
	}
}

namespace ImGui {
	
	extern bool ShowAnimationComponentDebugWindow(erm::Game& game, erm::ecs::AnimationComponent& animationComponent);
	
}
