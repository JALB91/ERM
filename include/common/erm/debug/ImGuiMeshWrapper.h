#pragma once

namespace erm {
	class Game;
	class Mesh;
}

namespace ImGui {
	
	extern void ShowMeshDebugWindow(erm::Game& game, erm::Mesh& mesh, unsigned int meshId);
	
}
