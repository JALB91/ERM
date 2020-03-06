#pragma once

namespace erm {
	namespace ecs {
		struct TransformComponent;
	}
}

namespace ImGui {
	
	extern bool ShowTransformComponentDebugWindow(erm::ecs::TransformComponent& transformComponent);
	
}
