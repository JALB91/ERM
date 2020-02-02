#pragma once

namespace erm {
	namespace ecs {
		struct TransformComponent;
	}
}

namespace ImGui {
	
	extern void ShowTransformComponentDebugWindow(erm::ecs::TransformComponent& transformComponent);
	
}
