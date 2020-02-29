#include "debug/ImGuiComponentWrapper.h"
#include "debug/ImGuiTransformComponentWrapper.h"
#include "debug/ImGuiModelComponentWrapper.h"
#include "debug/ImGuiCameraComponentWrapper.h"
#include "debug/ImGuiLightComponentWrapper.h"
#include "debug/ImGuiSkeletonComponentWrapper.h"
#include "debug/ImGuiAnimationComponentWrapper.h"

#include "game/Game.h"

#include "window/Window.h"

#include "ecs/ECS.h"
#include "ecs/Entity.h"
#include "ecs/EntityId.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"
#include "ecs/systems/LightSystem.h"
#include "ecs/systems/SkeletonSystem.h"
#include "ecs/systems/AnimationSystem.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowComponentDebugWindow(erm::Game& game, erm::ecs::EntityId entity)
	{
		if (!entity.IsValid()) return;
		
		bool hasTransform = false;
		bool hasModel = false;
		bool hasCamera = false;
		bool hasLight = false;
		bool hasSkeleton = false;
//		bool hasAnimation = false;
		
		if (erm::ecs::TransformComponent* transformComponent = game.GetECS().GetSystem<erm::ecs::TransformSystem>().GetComponent(entity))
		{
			if (ShowTransformComponentDebugWindow(*transformComponent))
			{
				game.GetECS().GetSystem<erm::ecs::TransformSystem>().RemoveComponent(entity);
			}
			hasTransform = true;
		}
		if (erm::ecs::ModelComponent* modelComponent = game.GetECS().GetSystem<erm::ecs::ModelSystem>().GetComponent(entity))
		{
			if (ShowModelComponentDebugWindow(game, *modelComponent))
			{
				game.GetECS().GetSystem<erm::ecs::ModelSystem>().RemoveComponent(entity);
			}
			hasModel = true;
		}
		if (erm::ecs::CameraComponent* cameraComponent = game.GetECS().GetSystem<erm::ecs::CameraSystem>().GetComponent(entity))
		{
			if (ShowCameraComponentDebugWindow(*cameraComponent))
			{
				game.GetECS().GetSystem<erm::ecs::CameraSystem>().RemoveComponent(entity);
			}
			hasCamera = true;
		}
		if (erm::ecs::LightComponent* lightComponent = game.GetECS().GetSystem<erm::ecs::LightSystem>().GetComponent(entity))
		{
			if (ShowLightComponentDebugWindow(*lightComponent))
			{
				game.GetECS().GetSystem<erm::ecs::LightSystem>().RemoveComponent(entity);
			}
			hasLight = true;
		}
		if (erm::ecs::SkeletonComponent* skeletonComponent = game.GetECS().GetSystem<erm::ecs::SkeletonSystem>().GetComponent(entity))
		{
			if (ShowSkeletonComponentDebugWindow(game, *skeletonComponent))
			{
				game.GetECS().GetSystem<erm::ecs::SkeletonSystem>().RemoveComponent(entity);
			}
			hasSkeleton = true;
		}
//		if (erm::ecs::AnimationComponent* animationComponent = game.GetECS().GetSystem<erm::ecs::AnimationSystem>().GetComponent(entity))
//		{
//			if (ShowAnimationComponentDebugWindow(*animationComponent))
//			{
//				game.GetECS().GetSystem<erm::ecs::AnimationSystem>().RemoveComponent(entity);
//			}
//			hasAnimation = true;
//		}
		
		ImGui::Separator();
		
		const bool hasMissingComponents = (!hasTransform || !hasModel || !hasCamera);
		
		if (hasMissingComponents && ImGui::Button("Add Component..."))
		{
			ImGui::OpenPopup("Components");
		}
		
		if (hasMissingComponents && ImGui::BeginPopup("Components"))
		{
			if (!hasTransform)
			{
				if (ImGui::Button("Transform"))
				{
					game.GetECS().GetEntityById(entity)->AddComponent<erm::ecs::TransformComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			if (!hasModel)
			{
				if (ImGui::Button("Model"))
				{
					game.GetECS().GetEntityById(entity)->AddComponent<erm::ecs::ModelComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			if (!hasCamera)
			{
				if (ImGui::Button("Camera"))
				{
					game.GetECS().GetEntityById(entity)->AddComponent<erm::ecs::CameraComponent>(game.GetWindow());
					ImGui::CloseCurrentPopup();
				}
			}
			
			if (!hasLight)
			{
				if (ImGui::Button("Light"))
				{
					game.GetECS().GetEntityById(entity)->AddComponent<erm::ecs::LightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			if (!hasSkeleton)
			{
				if (ImGui::Button("Skeleton"))
				{
					game.GetECS().GetEntityById(entity)->AddComponent<erm::ecs::SkeletonComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}
	
}
