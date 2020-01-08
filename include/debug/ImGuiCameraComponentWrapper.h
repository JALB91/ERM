#pragma once

namespace erm {
	namespace ecs {
		struct CameraComponent;
	}
}

namespace ImGui {
	
	void ShowCameraComponentDebugWindow(erm::ecs::CameraComponent& cameraComponent);
	
}
