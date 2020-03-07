#include "erm/debug/ImGuiWrapper.h"
#include "erm/debug/ImGuiEntitiesWrapper.h"
#include "erm/debug/ImGuiRenderingWrapper.h"
#include "erm/debug/ImGuiMainMenuBarWrapper.h"

namespace ImGui {
	
	void ShowGameDebug(erm::Game& game)
	{
		ImGui::ShowMainMenuBar(game);
		ImGui::ShowEntitiesDebugWindow(game);
		ImGui::ShowRenderingDebugWindow(game);
	}
	
}
