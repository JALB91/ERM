#include "erm/editor/ImGuiSkeletonComponentWrapper.h"

#include "erm/editor/ImGuiUtils.h"

#include <erm/assets/data_structs/Skeleton.h>
//#include <erm/assets/ResourcesManager.h>
#include <erm/ecs/systems/SkeletonSystem.h>
#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>

#include <imgui.h>

namespace ImGui {

void ShowPathOptions(erm::ecs::SkeletonComponent& skeletonComponent);

bool ShowSkeletonComponentDebugWindow(erm::ecs::SkeletonComponent& skeletonComponent)
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

		ShowPathOptions(skeletonComponent);

		/*if (erm::Skeleton* skeleton = skeletonComponent.getSkin())
		{
			bool displayBones = skeletonComponent.getDisplayBones();
			if (ImGui::Checkbox("Display Bones: ", &displayBones))
				skeletonComponent.setDisplayBones(displayBones);

			erm::BonesTree& rootBone = skeleton->mRootBone;

			if (ImGui::Button("Reset"))
			{
				rootBone.forEachDo([](erm::BonesTree& node) {
					erm::Bone& bone = node.getPayload();
					bone.mLocalTransform = bone.mBindTransform;
				});
				skeletonComponent.setDirty(true);
			}

			bool hasChanges = false;
			rootBone.forEachDo(
				[&hasChanges](erm::BonesTree& node) {
					ImGui::PushID(static_cast<int>(node.getId()));
					if (node.getParent())
						ImGui::Indent(ImGui::GetStyle().IndentSpacing * 0.75f);
					if (ImGui::TreeNode(node.getPayload().mName.getDebugString().c_str()))
					{
						hasChanges |= ImGui::ShowMatrixDebug(node.getPayload().mLocalTransform);
						ImGui::TreePop();
					}
				},
				[](erm::BonesTree& node) {
					if (node.getParent())
						ImGui::Unindent(ImGui::GetStyle().IndentSpacing * 0.75f);
					ImGui::PopID();
				});
			if (hasChanges)
			{
				skeletonComponent.setDirty(true);
			}
		}*/

		ImGui::Unindent();
	}

	return shouldRemove;
}

void ShowPathOptions(erm::ecs::SkeletonComponent& /*skeletonComponent*/)
{
	/*const erm::Skins& all = engine.GetResourcesManager().GetSkins();

	erm::Skeleton* skeleton = skeletonComponent.getSkin();
	std::string currentPath = skeleton ? skeleton->mPath : "";

	if (ImGui::BeginCombo("Path", currentPath.c_str()))
	{
		bool isSelected = currentPath == "";
		ImGui::PushID("NO_SELECTABLE");
		if (ImGui::Selectable("", &isSelected))
		{
			currentPath = "";
			skeletonComponent.setSkin(nullptr);
		}
		ImGui::PopID();

		for (unsigned int i = 0; i < all.size(); ++i)
		{
			const std::string& currentName = all[i]->mPath;
			isSelected = currentPath == currentName;
			if (ImGui::Selectable(currentName.c_str(), &isSelected))
			{
				if (currentPath != currentName)
				{
					currentPath = currentName;
					skeletonComponent.setSkin(engine.GetResourcesManager().GetSkin(currentPath.c_str()));
				}
			}
		}
		ImGui::EndCombo();
	}*/
}

} // namespace ImGui
