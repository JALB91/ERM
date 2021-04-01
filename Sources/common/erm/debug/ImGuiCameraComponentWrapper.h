#pragma once

namespace erm {
namespace ecs {
struct CameraComponent;
}
} // namespace erm

namespace ImGui {

extern bool ShowCameraComponentDebugWindow(erm::ecs::CameraComponent& cameraComponent);

}
