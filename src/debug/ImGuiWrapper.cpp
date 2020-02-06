#include "debug/ImGuiWrapper.h"
#include "debug/ImGuiEntitiesWrapper.h"
#include "debug/ImGuiRenderingWrapper.h"
#include "debug/ImGuiMainMenuBarWrapper.h"

namespace ImGui {
	
	void ShowGameDebug()
	{
		ImGui::ShowMainMenuBar();
		ImGui::ShowEntitiesDebugWindow();
		ImGui::ShowRenderingDebugWindow();
	}
	
}
