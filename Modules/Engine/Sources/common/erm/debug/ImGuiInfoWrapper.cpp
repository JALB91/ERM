#include "erm/debug/ImGuiInfoWrapper.h"
#include "erm/debug/ImGuiUtils.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/window/Window.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Timer.h"

#include <imgui.h>

#include <algorithm>

namespace ImGui {

enum class TREE_OP
{
	NONE,
	EXPAND,
	COLLAPSE
};

static const std::vector<std::pair<const char*, unsigned int>> kFPSOptions {
	{"30", 30},
	{"60", 60},
	{"120", 120},
	{"144", 144}
};

void ShowProfilingTree(const erm::Profiler::ProfilingTree& node, TREE_OP operation);

void ShowInfoWindow(erm::Engine& engine, bool& open)
{
	const erm::Timer& timer = engine.GetTimer();
	const erm::Window& window = engine.GetWindow();

	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.GetWindowWidth() / 2), static_cast<float>(window.GetWindowHeight() / 2)), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Info", &open, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Current Time: %f", timer.GetCurrentTime());
		ImGui::Text("Total Elapsed Time %f", timer.GetElapsedTime());
		ImGui::Text("Update Elapsed Time: %f", timer.GetUpdateElapsedTime());

		ImGui::Separator();

		ImGui::Text("FPS: %d", engine.GetFPS());

		ImGui::SameLine();

		unsigned int maxFPS = engine.GetMaxFPS();
		if (ImGui::ShowComboOf(kFPSOptions, "Max FPS", maxFPS))
			engine.SetMaxFPS(maxFPS);

		ImGui::Separator();

		if (const erm::Profiler::ProfilingTree* root = erm::Profiler::GetRoot())
		{
			TREE_OP operation = TREE_OP::NONE;

			if (ImGui::Button("Expand All"))
				operation = TREE_OP::EXPAND;
			ImGui::SameLine();
			if (ImGui::Button("Collapse All"))
				operation = TREE_OP::COLLAPSE;
			
			ShowProfilingTree(*root, operation);
		}

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
	ImGui::Text("%.4f", node.GetPayload().mTime);
	if (nodeOpen)
	{
		std::for_each(children.cbegin(), children.cend(), [operation](auto& child) {
			ShowProfilingTree(*child, operation);
		});
		ImGui::TreePop();
	}
}

} // namespace ImGui
