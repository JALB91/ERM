#pragma once

namespace erm::ecs {
struct ModelComponent;
} // namespace erm::ecs

namespace ImGui {

extern bool ShowModelComponentDebugWindow(erm::ecs::ModelComponent& modelComponent);

}
