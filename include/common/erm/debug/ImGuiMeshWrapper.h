#pragma once

namespace erm {
	class Engine;
	class Mesh;
}

namespace ImGui {
	
	extern void ShowMeshDebugWindow(erm::Engine& engine, erm::Mesh& mesh, unsigned int meshId);
	
}
