#include "debug/ImGuiComponentWrapper.h"
#include "debug/ImGuiTransformComponentWrapper.h"
#include "debug/ImGuiModelComponentWrapper.h"
#include "debug/ImGuiCameraComponentWrapper.h"

#include "game/Game.h"

#include "window/Window.h"

#include "ecs/ECS.h"
#include "ecs/Entity.h"
#include "ecs/EntityId.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/ModelComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowComponentDebugWindow(erm::ecs::EntityId entity)
	{
		if (!entity.IsValid()) return;
		
		erm::Game& game = erm::Game::GetInstance();
		bool hasTransform = false;
		bool hasModel = false;
		bool hasCamera = false;
		
		if (erm::ecs::TransformComponent* transformComponent = game.GetECS().GetSystem<erm::ecs::TransformSystem>().GetComponent(entity()))
		{
			ShowTransformComponentDebugWindow(*transformComponent);
			hasTransform = true;
		}
		if (erm::ecs::ModelComponent* modelComponent = game.GetECS().GetSystem<erm::ecs::ModelSystem>().GetComponent(entity()))
		{
			ShowModelComponentDebugWindow(*modelComponent);
			hasModel = true;
		}
		if (erm::ecs::CameraComponent* cameraComponent = game.GetECS().GetSystem<erm::ecs::CameraSystem>().GetComponent(entity()))
		{
			ShowCameraComponentDebugWindow(*cameraComponent);
			hasCamera = true;
		}
		
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
			ImGui::EndPopup();
		}
	}
	
}
