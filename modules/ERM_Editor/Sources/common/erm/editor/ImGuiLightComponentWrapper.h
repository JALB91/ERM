#pragma once

namespace erm::ecs {
struct LightComponent;
} // namespace erm::ecs

namespace ImGui {

extern bool ShowLightComponentDebugWindow(erm::ecs::LightComponent& lightComponent);

}
