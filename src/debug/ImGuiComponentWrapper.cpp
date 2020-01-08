#include "debug/ImGuiComponentWrapper.h"
#include "debug/ImGuiTransformComponentWrapper.h"
#include "debug/ImGuiModelComponentWrapper.h"
#include "debug/ImGuiCameraComponentWrapper.h"

#include "ecs/ECS.h"
#include "ecs/EntityId.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/ModelComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"

namespace ImGui {
	
	void ShowComponentDebugWindow(erm::ecs::ECS& ecs, erm::ecs::EntityId entity)
	{
		if (!entity.IsValid()) return;
		
		if (erm::ecs::TransformComponent* transformComponent = ecs.GetSystem<erm::ecs::TransformSystem>().GetComponent(entity()))
		{
			ShowTransformComponentDebugWindow(*transformComponent);
		}
		if (erm::ecs::ModelComponent* modelComponent = ecs.GetSystem<erm::ecs::ModelSystem>().GetComponent(entity()))
		{
			ShowModelComponentDebugWindow(*modelComponent);
		}
		if (erm::ecs::CameraComponent* cameraComponent = ecs.GetSystem<erm::ecs::CameraSystem>().GetComponent(entity()))
		{
			ShowCameraComponentDebugWindow(*cameraComponent);
		}
	}
	
}
