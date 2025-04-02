#pragma once

namespace erm {
namespace ecs {
struct LightComponent;
}
} // namespace erm

namespace ImGui {

extern bool ShowLightComponentDebugWindow(erm::ecs::LightComponent& lightComponent);

}
