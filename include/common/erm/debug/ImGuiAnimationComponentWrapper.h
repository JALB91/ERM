#pragma once

namespace erm {
	class Engine;
	namespace ecs {
		struct AnimationComponent;
	}
} // namespace erm

namespace ImGui {

	extern bool ShowAnimationComponentDebugWindow(erm::Engine& engine, erm::ecs::AnimationComponent& animationComponent);

}
