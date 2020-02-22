#include "debug/ImGuiEntitiesWrapper.h"
#include "debug/ImGuiEntityWrapper.h"
#include "debug/ImGuiComponentWrapper.h"

#include "game/Game.h"

#include "window/Window.h"

#include "ecs/EntityId.h"
#include "ecs/ECSConfig.h"
#include "ecs/IComponent.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowEntitiesDebugWindow(erm::Game& game)
	{
		const erm::Window& window = game.GetWindow();
		const erm::math::vec2 winSize (window.GetWindowWidth(), window.GetWindowHeight());
		const erm::math::vec2& viewport = window.GetViewport();
		
		static erm::ecs::EntityId active;
		
		const int flags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus;
		
		if (ImGui::Begin("Scene", nullptr, flags))
		{
			active = ImGui::ShowEntityDebugWindow(game.GetECS(), active, erm::ecs::EntityId(erm::ecs::ROOT_ID));
		}
		ImGui::SetWindowSize(ImVec2((winSize.x - viewport.x) * 0.5f, winSize.y - ImGui::GetFrameHeight()));
		ImGui::SetWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
		ImGui::End();
		
		if (ImGui::Begin("Entity", nullptr, flags) && active.IsValid())
		{
			ImGui::PushID(active());
			ImGui::Separator();
			ImGui::Text("Components");
			ImGui::ShowComponentDebugWindow(game, active);
			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::SetWindowSize(ImVec2((winSize.x - viewport.x) * 0.5f, winSize.y - ImGui::GetFrameHeight()));
		ImGui::SetWindowPos(ImVec2(winSize.x - ((winSize.x - viewport.x) * 0.5f), ImGui::GetFrameHeight()));
		ImGui::End();
	}
	
}
