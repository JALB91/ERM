#pragma once

namespace erm {
	namespace ecs {
		struct LightComponent;
	}
}

namespace ImGui {
	
	extern bool ShowLightComponentDebugWindow(erm::ecs::LightComponent& lightComponent);
	
}
