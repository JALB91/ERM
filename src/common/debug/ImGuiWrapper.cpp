#include "erm/debug/ImGuiWrapper.h"
#include "erm/debug/ImGuiEntitiesWrapper.h"
#include "erm/debug/ImGuiMainMenuBarWrapper.h"
#include "erm/debug/ImGuiRenderingWrapper.h"

namespace ImGui {

	void ShowEngineDebug(erm::Engine& engine)
	{
		ImGui::ShowMainMenuBar(engine);
		ImGui::ShowEntitiesDebugWindow(engine);
		ImGui::ShowRenderingDebugWindow(engine);
	}

} // namespace ImGui
