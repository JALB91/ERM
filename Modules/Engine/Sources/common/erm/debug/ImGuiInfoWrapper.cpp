#include "erm/debug/ImGuiInfoWrapper.h"
#include "erm/debug/ImGuiUtils.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/window/Window.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Sampler.h"
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
		
		ImGui::Text("Avg Sim time:");
		ImGui::SameLine(ImGui::GetWindowSize().x - 50.0f);
		ImGui::Text("%.4f", erm::gSimSampler.GetAverage());
		
		ImGui::Separator();
		
		ImGui::Text("Avg Render time:");
		ImGui::SameLine(ImGui::GetWindowSize().x - 50.0f);
		ImGui::Text("%.4f", erm::gRenderSampler.GetAverage());
		
		ImGui::Separator();

		if (const auto* profilingTreeRoot = erm::Profiler::GetProfilingTreeRoot())
		{
			TREE_OP operation = TREE_OP::NONE;

			if (ImGui::Button("Expand All"))
				operation = TREE_OP::EXPAND;
			ImGui::SameLine();
			if (ImGui::Button("Collapse All"))
				operation = TREE_OP::COLLAPSE;
			
			ShowProfilingTree(*profilingTreeRoot, operation);
		}

		ImGui::Separator();
	}

	ImGui::End();
}

void ShowProfilingTree(const erm::Profiler::ProfilingTree& node, TREE_OP operation)
{
	ImGui::Separator();

	const auto& children = node.GetChildren();
	ImGuiTreeNodeFlags flags = 0;
	if (children.empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}
	
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

	const bool nodeOpen = ImGui::TreeNodeEx(node.GetId().data(), flags);
	ImGui::SameLine(ImGui::GetWindowSize().x - 50.0f);
	ImGui::Text("%.4f", node.GetPayload().mSampler.GetAverage());
	if (nodeOpen || operation == TREE_OP::COLLAPSE)
	{
		if (operation == TREE_OP::COLLAPSE && !children.empty())
			TreePush(node.GetId().data());
		
		std::for_each(children.cbegin(), children.cend(), [operation](auto& child) {
			ShowProfilingTree(*child, operation);
		});
		
		ImGui::TreePop();
	}
}

} // namespace ImGui
