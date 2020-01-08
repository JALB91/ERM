#pragma once

namespace erm {
	namespace ecs {
		struct TransformComponent;
	}
}

namespace ImGui {
	
	void ShowTransformComponentDebugWindow(erm::ecs::TransformComponent& transformComponent);
	
}
