#pragma once

namespace erm {
	namespace ecs {
		class ECS;
		struct EntityId;
	}
}

namespace ImGui {
	
	extern void ShowComponentDebugWindow(erm::ecs::ECS& ecs, erm::ecs::EntityId entity);
	
}
