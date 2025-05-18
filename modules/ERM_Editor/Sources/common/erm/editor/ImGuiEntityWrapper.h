#pragma once

namespace erm::ecs {
class ECS;
struct EntityId;
} // namespace erm::ecs

namespace ImGui {

extern erm::ecs::EntityId ShowEntityDebugWindow(
	erm::ecs::EntityId active,
	erm::ecs::EntityId entity);

}
