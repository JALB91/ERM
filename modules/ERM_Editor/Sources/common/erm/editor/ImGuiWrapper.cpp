#include "erm/editor/ImGuiWrapper.h"
#include "erm/editor/ImGuiEntitiesWrapper.h"
#include "erm/editor/ImGuiGameWindow.h"
#include "erm/editor/ImGuiMainMenuBarWrapper.h"
#include "erm/editor/ImGuiRenderingWrapper.h"

namespace ImGui {

void ShowEngineDebug()
{
	ImGui::ShowMainMenuBar();
	ImGui::ShowEntitiesDebugWindow();
	ImGui::ShowRenderingDebugWindow();
	ImGui::ShowGameWindow();
}

} // namespace ImGui
