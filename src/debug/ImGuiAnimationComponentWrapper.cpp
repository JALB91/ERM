#include "debug/ImGuiAnimationComponentWrapper.h"

#include "ecs/systems/AnimationSystem.h"

#include "managers/ResourcesManager.h"

#include "game/Game.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowPathOptions(erm::Game& game, erm::ecs::AnimationComponent& animationComponent);
	
	bool ShowAnimationComponentDebugWindow(erm::Game& game, erm::ecs::AnimationComponent& animationComponent)
	{
		const bool headerOpen = ImGui::CollapsingHeader("Animation");
		
		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup("AnimationPopup");
		}
		
		bool shouldRemove = false;
		
		if (ImGui::BeginPopup("AnimationPopup"))
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
			
			ShowPathOptions(game, animationComponent);
			
			if (animationComponent.mSkeletonAnimation)
			{
				ImGui::LabelText("Animation Name", "%s", animationComponent.mSkeletonAnimation->mName.c_str());
				ImGui::Checkbox("Play", &animationComponent.mPlaying);
				ImGui::DragFloat("Time Scale", &animationComponent.mTimeScale, 0.05f);
				ImGui::ProgressBar(animationComponent.mCurrentAnimationTime / animationComponent.mSkeletonAnimation->mTotalAnimationTime);
			}
			
			ImGui::Unindent();
		}
		
		return shouldRemove;
	}
	
	void ShowPathOptions(erm::Game& game, erm::ecs::AnimationComponent& animationComponent)
	{
		const erm::Animations& all = game.GetResourcesManager().GetLoadedAnimations();
		
		erm::SkeletonAnimation* skeletonAnimation = animationComponent.mSkeletonAnimation;
		std::string currentPath = skeletonAnimation ? skeletonAnimation->mName : "";
		
		if (ImGui::BeginCombo("Path", currentPath.c_str()))
		{
			bool isSelected = currentPath == "";
			if (ImGui::Selectable("", &isSelected))
			{
				currentPath = "";
				animationComponent.mSkeletonAnimation = nullptr;
			}
			
			for (unsigned int i = 0; i < all.size(); ++i)
			{
				const std::string& currentName = all[i]->mName;
				isSelected = currentPath == currentName;
				if (ImGui::Selectable(currentName.c_str(), &isSelected))
				{
					if (currentPath != currentName)
					{
						currentPath = currentName;
						animationComponent.mSkeletonAnimation = game.GetResourcesManager().GetAnimation(currentPath.c_str());
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	
}
