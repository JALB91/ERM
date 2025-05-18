#pragma once

namespace erm::ecs {
struct EntityId;
} // namespace erm::ecs

namespace ImGui {

extern void ShowComponentDebugWindow(erm::ecs::EntityId entity);

}
