#include "debug/ImGuiComponentWrapper.h"
#include "debug/ImGuiTransformComponentWrapper.h"
#include "debug/ImGuiModelComponentWrapper.h"
#include "debug/ImGuiCameraComponentWrapper.h"

#include "game/Game.h"

#include "ecs/ECS.h"
#include "ecs/EntityId.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/ModelComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"

namespace ImGui {
	
	void ShowComponentDebugWindow(erm::ecs::EntityId entity)
	{
		if (!entity.IsValid()) return;
		
		erm::Game& game = erm::Game::GetInstance();
		
		if (erm::ecs::TransformComponent* transformComponent = game.GetECS().GetSystem<erm::ecs::TransformSystem>().GetComponent(entity()))
		{
			ShowTransformComponentDebugWindow(*transformComponent);
		}
		if (erm::ecs::ModelComponent* modelComponent = game.GetECS().GetSystem<erm::ecs::ModelSystem>().GetComponent(entity()))
		{
			ShowModelComponentDebugWindow(*modelComponent);
		}
		if (erm::ecs::CameraComponent* cameraComponent = game.GetECS().GetSystem<erm::ecs::CameraSystem>().GetComponent(entity()))
		{
			ShowCameraComponentDebugWindow(*cameraComponent);
		}
	}
	
}
