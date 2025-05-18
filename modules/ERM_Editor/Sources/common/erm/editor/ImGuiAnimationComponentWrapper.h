#pragma once

namespace erm::ecs {
struct AnimationComponent;
} // namespace erm::ecs

namespace ImGui {

extern bool ShowAnimationComponentDebugWindow(erm::ecs::AnimationComponent& animationComponent);

}
