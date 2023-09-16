#include "erm/debug/ImGuiAnimationComponentWrapper.h"

#include "erm/ecs/systems/AnimationSystem.h"

#include "erm/resources/ResourcesManager.h"

#include "erm/engine/Engine.h"

#include "erm/resources/animations/SkeletonAnimation.h"

#include <imgui.h>

namespace ImGui {

void ShowPathOptions(erm::Engine& engine, erm::ecs::AnimationComponent& animationComponent);

bool ShowAnimationComponentDebugWindow(erm::Engine& engine, erm::ecs::AnimationComponent& animationComponent)
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

		ShowPathOptions(engine, animationComponent);

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

void ShowPathOptions(erm::Engine& engine, erm::ecs::AnimationComponent& animationComponent)
{
	const erm::Animations& all = engine.GetResourcesManager().GetAnimations();

	erm::SkeletonAnimation* skeletonAnimation = animationComponent.mSkeletonAnimation;
	std::string currentPath = skeletonAnimation ? skeletonAnimation->mName : "";

	if (ImGui::BeginCombo("Path", currentPath.c_str()))
	{
		bool isSelected = currentPath == "";
		ImGui::PushID("NO_SELECTABLE");
		if (ImGui::Selectable("", &isSelected))
		{
			currentPath = "";
			animationComponent.mSkeletonAnimation = nullptr;
		}
		ImGui::PopID();

		for (unsigned int i = 0; i < all.size(); ++i)
		{
			const std::string& currentName = all[i]->mName;
			isSelected = currentPath == currentName;
			if (ImGui::Selectable(currentName.c_str(), &isSelected))
			{
				if (currentPath != currentName)
				{
					currentPath = currentName;
					animationComponent.mSkeletonAnimation = engine.GetResourcesManager().GetAnimation(currentPath.c_str());
				}
			}
		}
		ImGui::EndCombo();
	}
}

} // namespace ImGui
