#include "debug/ImGuiMainMenuBarWrapper.h"

#include "debug/ImGuiShadersWrapper.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowMainMenuBar()
	{
		static bool showDemo = false;
		static bool showShaders = false;
		
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Demo", nullptr, &showDemo);
				ImGui::MenuItem("Shaders", nullptr, &showShaders);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		
		if (showDemo) ImGui::ShowDemoWindow(&showDemo);
		if (showShaders) ImGui::ShowShadersDebug(showShaders);
	}
	
}
