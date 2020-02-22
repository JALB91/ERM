#include "debug/ImGuiMainMenuBarWrapper.h"

#include "debug/ImGuiInfoWrapper.h"
#include "debug/ImGuiShadersWrapper.h"

#include "input/Keys.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowMainMenuBar(erm::Game& game)
	{
		static bool showDemo = false;
		static bool showInfos = false;
		static bool showShaders = false;
		
		if (ImGui::BeginMainMenuBar())
		{
			showDemo |= ImGui::MenuItem("Demo");
			showInfos |= ImGui::MenuItem("Infos");
			showShaders |= ImGui::MenuItem("Shaders");
			ImGui::EndMainMenuBar();
		}
		
		if (!showInfos) showInfos = ImGui::IsKeyPressed(erm::KEY_I);
		
		if (showDemo) ImGui::ShowDemoWindow(&showDemo);
		if (showInfos) ImGui::ShowInfoWindow(game, showInfos);
		if (showShaders) ImGui::ShowShadersDebug(game, showShaders);
	}
	
}
