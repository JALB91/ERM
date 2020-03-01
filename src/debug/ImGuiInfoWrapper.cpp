#include "debug/ImGuiInfoWrapper.h"

#include "game/Game.h"

#include "window/Window.h"

#include "utils/Timer.h"
#include "utils/Profiler.h"

#include <imgui.h>

#include <algorithm>

namespace ImGui {
	
	enum class TREE_OP
	{
		NONE,
		EXPAND,
		COLLAPSE
	};
	
	void ShowProfilingTree(const erm::Profiler::ProfilingTree& node, TREE_OP operation);
	
	void ShowInfoWindow(erm::Game& game, bool& open)
	{
		const erm::Timer& timer = game.GetTimer();
		const erm::Window& window = game.GetWindow();
		
		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.GetWindowWidth() / 2), static_cast<float>(window.GetWindowHeight() / 2)), ImGuiCond_FirstUseEver);
		
		if (ImGui::Begin("Info", &open, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::Text("Current Time: %f", timer.GetCurrentTime());
			ImGui::Text("Total Elapsed Time %f", timer.GetElapsedTime());
			ImGui::Text("Frame Elapsed Time: %f", timer.GetFrameElapsedTime());
			ImGui::Text("FPS: %d", game.GetFPS());
			
			ImGui::Separator();
			
			TREE_OP operation = TREE_OP::NONE;
			
			if (ImGui::Button("Expand All")) operation = TREE_OP::EXPAND;
			ImGui::SameLine();
			if (ImGui::Button("Collapse All")) operation = TREE_OP::COLLAPSE;
			
			ShowProfilingTree(erm::Profiler::GetRoot(), operation);
			
			ImGui::Separator();
		}
		
		ImGui::End();
	}
	
	void ShowProfilingTree(const erm::Profiler::ProfilingTree& node, TREE_OP operation)
	{
		switch (operation)
		{
			case TREE_OP::EXPAND:
				ImGui::SetNextItemOpen(true);
				break;
			case TREE_OP::COLLAPSE:
				ImGui::SetNextItemOpen(false);
				break;
			default:
				break;
		}
		
		ImGui::Separator();
		
		const auto& children = node.GetChildren();
		int flags = 0;
		if (children.empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		
		const bool nodeOpen = ImGui::TreeNodeEx(node.GetId().c_str(), flags);
		ImGui::SameLine(ImGui::GetWindowSize().x - 50.0f);
		ImGui::Text("%.2f", node.GetPayload().mTime);
		if (nodeOpen)
		{
			std::for_each(children.cbegin(), children.cend(), [operation](auto& child) {
				ShowProfilingTree(child, operation);
			});
			ImGui::TreePop();
		}
	}
	
}
