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
	
	template<typename T>
	void ShowComponentDebugWindow(erm::Game& game, erm::ecs::EntityId entity, const std::function<bool(T&)>& callback, const char* name)
	{
		if (T* component = game.GetECS().GetSystem<typename T::SYSTEM_TYPE>().GetComponent(entity))
		{
			ImGui::PushID(name);
			if (callback(*component))
			{
				game.GetECS().GetSystem<typename T::SYSTEM_TYPE>().RemoveComponent(entity);
			}
			ImGui::PopID();
		}
		else if (ImGui::BeginPopup("Components"))
		{
			if (ImGui::Button(name))
			{
				game.GetECS().GetEntityById(entity)->AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	
	void ShowComponentDebugWindow(erm::Game& game, erm::ecs::EntityId entity)
	{
		if (!entity.IsValid()) return;
		
		if (ImGui::Button("Add Component..."))
		{
			ImGui::OpenPopup("Components");
		}
		
		ImGui::Separator();
		
		ShowComponentDebugWindow<erm::ecs::TransformComponent>(game, entity, [](erm::ecs::TransformComponent& component) { return ShowTransformComponentDebugWindow(component); }, "Transform");
		ShowComponentDebugWindow<erm::ecs::ModelComponent>(game, entity, [&game](erm::ecs::ModelComponent& component) { return ShowModelComponentDebugWindow(game, component); }, "Model");
		ShowComponentDebugWindow<erm::ecs::CameraComponent>(game, entity, [](erm::ecs::CameraComponent& component) { return ShowCameraComponentDebugWindow(component); }, "Camera");
		ShowComponentDebugWindow<erm::ecs::LightComponent>(game, entity, [](erm::ecs::LightComponent& component) { return ShowLightComponentDebugWindow(component); }, "Light");
		ShowComponentDebugWindow<erm::ecs::SkeletonComponent>(game, entity, [&game](erm::ecs::SkeletonComponent& component) { return ShowSkeletonComponentDebugWindow(game, component); }, "Skeleton");
		ShowComponentDebugWindow<erm::ecs::AnimationComponent>(game, entity, [&game](erm::ecs::AnimationComponent& component) { return ShowAnimationComponentDebugWindow(game, component); }, "Animation");
	}
	
}
