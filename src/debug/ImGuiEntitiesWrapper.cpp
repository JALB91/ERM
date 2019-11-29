#include "debug/ImGuiEntitiesWrapper.h"
#include "debug/ImGuiEntityWrapper.h"
#include "debug/ImGuiComponentWrapper.h"

#include "game/Game.h"

#include "window/Window.h"

#include "ec/Entity.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowEntitiesDebugWindow(erm::Game& game)
	{
		const erm::Window& window = game.GetWindow();
		const erm::math::vec2 winSize (window.GetWindowWidth(), window.GetWindowHeight());
		const erm::math::vec2& viewport = window.GetViewport();
		
		static erm::Entity* active = nullptr;
		
		if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::ShowEntityDebugWindow(&active, game.GetRoot());
		}
		ImGui::SetWindowSize(ImVec2((winSize.x - viewport.x) * 0.5f, winSize.y - ImGui::GetFrameHeight()));
		ImGui::SetWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
		ImGui::End();
		
		if (ImGui::Begin("Entity", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar) && active)
		{
			ImGui::PushID(active->GetEntityID());
			ImGui::Separator();
			ImGui::Text("Components");
			active->ForEachComponent([](erm::IComponent& component) {
				ImGui::ShowComponentDebugWindow(&component);
			});
			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::SetWindowSize(ImVec2((winSize.x - viewport.x) * 0.5f, winSize.y - ImGui::GetFrameHeight()));
		ImGui::SetWindowPos(ImVec2(winSize.x - ((winSize.x - viewport.x) * 0.5f), ImGui::GetFrameHeight()));
		ImGui::End();
	}
	
}
