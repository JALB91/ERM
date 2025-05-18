#include "erm/editor/ImGuiInfoWrapper.h"
#include "erm/editor/ImGuiUtils.h"

#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/utils/Profiler.h>
#include <erm/utils/Sampler.h>
#include <erm/utils/Timer.h>
#include <erm/window/Window.h>

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

void ShowInfoWindow(bool& open)
{
	auto& engine = *erm::ObjectRegistry::get<erm::Engine>();
	const erm::Timer& timer = engine.getTimer();
	const erm::Window& window = *erm::ObjectRegistry::get<erm::Window>();

	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.getWindowWidth() / 2), static_cast<float>(window.getWindowHeight() / 2)), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Info", &open, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Current Time: %f", timer.getCurrentTime());
		ImGui::Text("Total Elapsed Time %f", timer.getElapsedTime());
		ImGui::Text("Update Elapsed Time: %f", timer.getUpdateElapsedTime());

		ImGui::Separator();

		ImGui::Text("FPS: %d", engine.getFPS());

		ImGui::SameLine();

		unsigned int maxFPS = engine.getMaxFPS();
		if (ImGui::ShowComboOf(kFPSOptions, "Max FPS", maxFPS))
			engine.setMaxFPS(maxFPS);

		ImGui::Separator();
		
		ImGui::Text("Avg Sim time:");
		ImGui::SameLine(ImGui::GetWindowSize().x - 50.0f);
		ImGui::Text("%.4f", erm::gSimSampler.getAverage());
		
		ImGui::Separator();
		
		ImGui::Text("Avg Render time:");
		ImGui::SameLine(ImGui::GetWindowSize().x - 50.0f);
		ImGui::Text("%.4f", erm::gRenderSampler.getAverage());
		
		ImGui::Separator();

		if (const auto* profilingTreeRoot = erm::Profiler::getProfilingTreeRoot())
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

	const auto& children = node.getChildren();
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

	const bool nodeOpen = ImGui::TreeNodeEx(node.getId().data(), flags);
	ImGui::SameLine(ImGui::GetWindowSize().x - 50.0f);
	ImGui::Text("%.4f", node.getPayload().mSampler.getAverage());
	if (nodeOpen || operation == TREE_OP::COLLAPSE)
	{
		if (operation == TREE_OP::COLLAPSE && !children.empty())
			TreePush(node.getId().data());
		
		std::for_each(children.cbegin(), children.cend(), [operation](auto& child) {
			ShowProfilingTree(child, operation);
		});
		
		ImGui::TreePop();
	}
}

} // namespace ImGui
