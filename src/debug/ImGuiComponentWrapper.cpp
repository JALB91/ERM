#include "debug/ImGuiComponentWrapper.h"
#include "debug/ImGuiTransformComponentWrapper.h"
#include "debug/ImGuiModelComponentWrapper.h"
#include "debug/ImGuiCameraComponentWrapper.h"

#include "ecs/ECS.h"
#include "ecs/Entity.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/ModelComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"

namespace ImGui {
	
	void ShowComponentDebugWindow(erm::ecs::Entity* entity)
	{
		if (!entity || !(*entity)) return;
		
		if (erm::ecs::TransformComponent* transformComponent = entity->GetECS().GetSystem<erm::ecs::TransformSystem>().GetComponent(entity->GetID()))
		{
			ShowTransformComponentDebugWindow(*transformComponent);
		}
		if (erm::ecs::ModelComponent* modelComponent = entity->GetECS().GetSystem<erm::ecs::ModelSystem>().GetComponent(entity->GetID()))
		{
			ShowModelComponentDebugWindow(*modelComponent);
		}
		if (erm::ecs::CameraComponent* cameraComponent = entity->GetECS().GetSystem<erm::ecs::CameraSystem>().GetComponent(entity->GetID()))
		{
			ShowCameraComponentDebugWindow(*cameraComponent);
		}
	}
	
}
