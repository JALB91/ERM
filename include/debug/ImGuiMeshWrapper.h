#pragma once

namespace erm {
	class Mesh;
}

namespace ImGui {
	
	void ShowMeshDebugWindow(erm::Mesh& mesh, unsigned int meshId);
	
}
