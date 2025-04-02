#include "erm/editor/ImGuiWrapper.h"
#include "erm/editor/ImGuiEntitiesWrapper.h"
#include "erm/editor/ImGuiGameWindow.h"
#include "erm/editor/ImGuiMainMenuBarWrapper.h"
#include "erm/editor/ImGuiRenderingWrapper.h"

namespace ImGui {

void ShowEngineDebug(erm::Engine& engine)
{
	ImGui::ShowMainMenuBar(engine);
	ImGui::ShowEntitiesDebugWindow(engine);
	ImGui::ShowRenderingDebugWindow(engine);
	ImGui::ShowGameWindow(engine);
}

} // namespace ImGui
