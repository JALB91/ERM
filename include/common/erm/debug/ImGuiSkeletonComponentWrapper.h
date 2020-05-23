#pragma once

namespace erm {
	class Engine;
	namespace ecs {
		struct SkeletonComponent;
	}
} // namespace erm

namespace ImGui {

	extern bool ShowSkeletonComponentDebugWindow(erm::Engine& engine, erm::ecs::SkeletonComponent& skeletonComponent);

}
