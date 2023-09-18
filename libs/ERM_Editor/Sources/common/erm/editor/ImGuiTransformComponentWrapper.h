#pragma once

namespace erm {
namespace ecs {
struct TransformComponent;
}
} // namespace erm

namespace ImGui {

extern bool ShowTransformComponentDebugWindow(erm::ecs::TransformComponent& transformComponent);

}
