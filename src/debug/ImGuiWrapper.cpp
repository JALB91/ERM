#include "debug/ImGuiWrapper.h"
#include "debug/ImGuiEntitiesWrapper.h"
#include "debug/ImGuiRenderingWrapper.h"
#include "debug/ImGuiMainMenuBarWrapper.h"

namespace ImGui {
	
	void ShowGameDebug(erm::Game& game)
	{
		ImGui::ShowMainMenuBar();
		ImGui::ShowEntitiesDebugWindow(game);
		ImGui::ShowRenderingDebugWindow(game);
	}
	
}
