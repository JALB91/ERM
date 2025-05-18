#pragma once

namespace erm::ecs {
struct CameraComponent;
} // namespace erm::ecs

namespace ImGui {

extern bool ShowCameraComponentDebugWindow(erm::ecs::CameraComponent& cameraComponent);

}
