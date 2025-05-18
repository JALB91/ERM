#pragma once

namespace erm::ecs {
struct SkeletonComponent;
} // namespace erm::ecs

namespace ImGui {

extern bool ShowSkeletonComponentDebugWindow(erm::ecs::SkeletonComponent& skeletonComponent);

}
