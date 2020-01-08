#pragma once

namespace erm {
	namespace ecs {
		class ECS;
		struct EntityId;
	}
}

namespace ImGui {
	
	erm::ecs::EntityId ShowEntityDebugWindow(
		erm::ecs::ECS& ecs,
		erm::ecs::EntityId active,
		erm::ecs::EntityId entity
	);
	
}
