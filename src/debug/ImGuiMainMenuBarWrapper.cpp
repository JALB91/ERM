#include "debug/ImGuiMainMenuBarWrapper.h"

#include "debug/ImGuiInfoWrapper.h"
#include "debug/ImGuiShadersWrapper.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowMainMenuBar(erm::Game& game)
	{
		static bool showDemo = false;
		static bool showInfos = false;
		static bool showShaders = false;
		
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Demo", nullptr, &showDemo);
				ImGui::MenuItem("Infos", nullptr, &showInfos);
				ImGui::MenuItem("Shaders", nullptr, &showShaders);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		
		if (showDemo) ImGui::ShowDemoWindow(&showDemo);
		if (showInfos) ImGui::ShowInfoWindow(game, showInfos);
		if (showShaders) ImGui::ShowShadersDebug(game, showShaders);
	}
	
}
