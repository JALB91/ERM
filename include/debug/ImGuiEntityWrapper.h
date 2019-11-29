#pragma once

namespace erm {
	class Entity;
}

namespace ImGui {
	
	void ShowEntityDebugWindow(erm::Entity** active, erm::Entity& entity);
	
}
