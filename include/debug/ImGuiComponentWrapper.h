#pragma once

namespace erm {
	namespace ecs {
		class ECS;
		struct EntityId;
	}
}

namespace ImGui {
	
	void ShowComponentDebugWindow(erm::ecs::ECS& ecs, erm::ecs::EntityId entity);
	
}
