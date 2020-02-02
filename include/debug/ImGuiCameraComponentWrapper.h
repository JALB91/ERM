#pragma once

namespace erm {
	namespace ecs {
		struct CameraComponent;
	}
}

namespace ImGui {
	
	extern void ShowCameraComponentDebugWindow(erm::ecs::CameraComponent& cameraComponent);
	
}
