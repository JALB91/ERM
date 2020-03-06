#pragma once

namespace erm {
	namespace ecs {
		struct CameraComponent;
	}
}

namespace ImGui {
	
	extern bool ShowCameraComponentDebugWindow(erm::ecs::CameraComponent& cameraComponent);
	
}
