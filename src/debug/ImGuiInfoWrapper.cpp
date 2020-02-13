#include "debug/ImGuiInfoWrapper.h"

#include "game/Game.h"

#include "window/Window.h"

#include "utils/Timer.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowInfoWindow(bool& open)
	{
		const erm::Game& game = erm::Game::GetInstance();
		const erm::Timer& timer = game.GetTimer();
		const erm::Window& window = game.GetWindow();
		
		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.GetWindowWidth() / 2), static_cast<float>(window.GetWindowHeight() / 2)), ImGuiCond_FirstUseEver);
		
		if (ImGui::Begin("Info", &open, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::Text("Current Time: %f", timer.GetCurrentTime());
			ImGui::Text("Frame Elapsed Time: %f", timer.GetFrameElapsedTime());
			ImGui::Text("FPS: %d", game.GetFPS());
		}
		
		ImGui::End();
	}
	
}
