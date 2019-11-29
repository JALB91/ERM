#include "debug/ImGuiMainMenuBarWrapper.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowMainMenuBar()
	{
		static bool showDemo = false;
		
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Demo", nullptr, &showDemo);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		
		if (showDemo) ImGui::ShowDemoWindow();
	}
	
}
