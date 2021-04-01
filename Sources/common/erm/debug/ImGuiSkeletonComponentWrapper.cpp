#include "erm/debug/ImGuiSkeletonComponentWrapper.h"

#include "erm/debug/ImGuiUtils.h"

#include "erm/ecs/systems/SkeletonSystem.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/engine/Engine.h"

#include <imgui.h>

namespace ImGui {

void ShowPathOptions(erm::Engine& engine, erm::ecs::SkeletonComponent& skeletonComponent);

bool ShowSkeletonComponentDebugWindow(erm::Engine& engine, erm::ecs::SkeletonComponent& skeletonComponent)
{
	const bool headerOpen = ImGui::CollapsingHeader("Skeleton");

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("SkeletonPopup");
	}

	bool shouldRemove = false;

	if (ImGui::BeginPopup("SkeletonPopup"))
	{
		if (ImGui::Button("Remove..."))
		{
			shouldRemove = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (headerOpen)
	{
		ImGui::Indent();

		ShowPathOptions(engine, skeletonComponent);

		if (erm::Skin* skin = skeletonComponent.GetSkin())
		{
			bool displayBones = skeletonComponent.GetDisplayBones();
			ImGui::Checkbox("Display Bones: ", &displayBones);
			skeletonComponent.SetDisplayBones(displayBones);

			erm::BonesTree* rootBone = skin->mRootBone.get();
			bool hasChanges = false;
			rootBone->ForEachDo(
				[&hasChanges](erm::BonesTree& node) {
					ImGui::PushID(static_cast<int>(node.GetId()));
					if (node.GetParent())
						ImGui::Indent(ImGui::GetStyle().IndentSpacing * 0.75f);
					if (ImGui::TreeNode(node.GetPayload()->mName.c_str()))
					{
						hasChanges |= ImGui::ShowMatrixDebug(node.GetPayload()->mLocalTransform);
						ImGui::TreePop();
					}
				},
				[](erm::BonesTree& node) {
					if (node.GetParent())
						ImGui::Unindent(ImGui::GetStyle().IndentSpacing * 0.75f);
					ImGui::PopID();
				});
			if (hasChanges)
			{
				skeletonComponent.SetDirty(true);
			}
		}

		ImGui::Unindent();
	}

	return shouldRemove;
}

void ShowPathOptions(erm::Engine& engine, erm::ecs::SkeletonComponent& skeletonComponent)
{
	const erm::Skins& all = engine.GetResourcesManager().GetSkins();

	erm::Skin* skin = skeletonComponent.GetSkin();
	std::string currentPath = skin ? skin->mPath : "";

	if (ImGui::BeginCombo("Path", currentPath.c_str()))
	{
		bool isSelected = currentPath == "";
		if (ImGui::Selectable("", &isSelected))
		{
			currentPath = "";
			skeletonComponent.SetSkin(nullptr);
		}

		for (unsigned int i = 0; i < all.size(); ++i)
		{
			const std::string& currentName = all[i]->mPath;
			isSelected = currentPath == currentName;
			if (ImGui::Selectable(currentName.c_str(), &isSelected))
			{
				if (currentPath != currentName)
				{
					currentPath = currentName;
					skeletonComponent.SetSkin(engine.GetResourcesManager().GetSkin(currentPath.c_str()));
				}
			}
		}
		ImGui::EndCombo();
	}
}

} // namespace ImGui
