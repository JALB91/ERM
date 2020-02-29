#include "debug/ImGuiSkeletonComponentWrapper.h"

#include "debug/ImGuiUtils.h"

#include "ecs/systems/SkeletonSystem.h"

#include "managers/ResourcesManager.h"

#include "game/Game.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowPathOptions(erm::Game& game, erm::ecs::SkeletonComponent& skeletonComponent);
	
	bool ShowSkeletonComponentDebugWindow(erm::Game& game, erm::ecs::SkeletonComponent& skeletonComponent)
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
			
			ShowPathOptions(game, skeletonComponent);
			
			if (skeletonComponent.mRootBone)
			{
				skeletonComponent.mRootBone->ForEachDo(
					[](erm::BonesTree& node) {
						ImGui::PushID(node.GetId());
						if (node.GetParent()) ImGui::Indent(ImGui::GetStyle().IndentSpacing * 0.75f);
						if (ImGui::TreeNode(node.GetPayload()->mName.c_str()))
						{
							ImGui::ShowMatrixDebug(node.GetPayload()->mAnimatedTransform);
							ImGui::TreePop();
						}
					},
					[](erm::BonesTree& node) {
						if (node.GetParent()) ImGui::Unindent(ImGui::GetStyle().IndentSpacing * 0.75f);
						ImGui::PopID();
					}
				);
			}
			
			ImGui::Unindent();
		}
		
		return shouldRemove;
	}
	
	void ShowPathOptions(erm::Game& game, erm::ecs::SkeletonComponent& skeletonComponent)
	{
		const erm::Skins& all = game.GetResourcesManager().GetLoadedSkins();
		
		erm::BonesTree* rootBone = skeletonComponent.mRootBone;
		std::string currentPath = rootBone ? rootBone->GetPayload()->mName : "";
		
		if (ImGui::BeginCombo("Path", currentPath.c_str()))
		{
			bool isSelected = currentPath == "";
			if (ImGui::Selectable("", &isSelected))
			{
				currentPath = "";
				skeletonComponent.mRootBone = nullptr;
			}
			
			for (unsigned int i = 0; i < all.size(); ++i)
			{
				const std::string& currentName = all[i]->GetPayload()->mName;
				bool isSelected = currentPath == currentName;
				if (ImGui::Selectable(currentName.c_str(), &isSelected))
				{
					if (currentPath != currentName)
					{
						currentPath = currentName;
						skeletonComponent.mRootBone = game.GetResourcesManager().GetSkin(currentPath.c_str());
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	
}
