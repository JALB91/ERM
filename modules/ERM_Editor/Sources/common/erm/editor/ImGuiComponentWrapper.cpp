#include "erm/editor/ImGuiComponentWrapper.h"
#include "erm/editor/ImGuiAnimationComponentWrapper.h"
#include "erm/editor/ImGuiCameraComponentWrapper.h"
#include "erm/editor/ImGuiLightComponentWrapper.h"
#include "erm/editor/ImGuiModelComponentWrapper.h"
#include "erm/editor/ImGuiRenderingComponentWrapper.h"
#include "erm/editor/ImGuiSkeletonComponentWrapper.h"
#include "erm/editor/ImGuiTransformComponentWrapper.h"

#include <erm/engine/Engine.h>

#include <erm/window/Window.h>

#include <erm/ecs/ECS.h>
#include <erm/ecs/Entity.h>
#include <erm/ecs/EntityId.h>
#include <erm/ecs/systems/AnimationSystem.h>
#include <erm/ecs/systems/CameraSystem.h>
#include <erm/ecs/systems/LightSystem.h>
#include <erm/ecs/systems/ModelSystem.h>
#include <erm/ecs/systems/RenderingSystem.h>
#include <erm/ecs/systems/SkeletonSystem.h>
#include <erm/ecs/systems/TransformSystem.h>

#include <imgui.h>

namespace ImGui {

template<typename T>
void ShowComponentDebugWindow(erm::Engine& engine, erm::ecs::EntityId entity, const std::function<bool(T&)>& callback, const char* name)
{
	if (T* component = engine.getECS().getSystem<typename T::SYSTEM_TYPE>()->getComponent(entity))
	{
		ImGui::PushID(name);
		if (callback(*component))
		{
			engine.getECS().getSystem<typename T::SYSTEM_TYPE>()->removeComponent(entity);
		}
		ImGui::PopID();
	}
	else if (ImGui::BeginPopup("Components"))
	{
		if (ImGui::Button(name))
		{
			engine.getECS().getEntityById(entity)->addComponent<T>();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ShowComponentDebugWindow(erm::Engine& engine, erm::ecs::EntityId entity)
{
	if (!entity.isValid())
		return;

	if (ImGui::Button("Add Component..."))
	{
		ImGui::OpenPopup("Components");
	}

	ImGui::Separator();

	ShowComponentDebugWindow<erm::ecs::TransformComponent>(
		engine,
		entity,
		[](erm::ecs::TransformComponent& component) {
			return ShowTransformComponentDebugWindow(component);
		},
		"Transform");
	ShowComponentDebugWindow<erm::ecs::ModelComponent>(
		engine,
		entity,
		[&engine](erm::ecs::ModelComponent& component) {
			return ShowModelComponentDebugWindow(engine, component);
		},
		"Model");
	ShowComponentDebugWindow<erm::ecs::CameraComponent>(
		engine,
		entity,
		[](erm::ecs::CameraComponent& component) {
			return ShowCameraComponentDebugWindow(component);
		},
		"Camera");
	ShowComponentDebugWindow<erm::ecs::LightComponent>(
		engine,
		entity,
		[](erm::ecs::LightComponent& component) {
			return ShowLightComponentDebugWindow(component);
		},
		"Light");
	ShowComponentDebugWindow<erm::ecs::SkeletonComponent>(
		engine,
		entity,
		[&engine](erm::ecs::SkeletonComponent& component) {
			return ShowSkeletonComponentDebugWindow(engine, component);
		},
		"Skeleton");
	ShowComponentDebugWindow<erm::ecs::AnimationComponent>(
		engine,
		entity,
		[&engine](erm::ecs::AnimationComponent& component) {
			return ShowAnimationComponentDebugWindow(engine, component);
		},
		"Animation");
	ShowComponentDebugWindow<erm::ecs::RenderingComponent>(
		engine,
		entity,
		[](erm::ecs::RenderingComponent& component) {
			return ShowRenderingComponentDebug(component);
		},
		"Rendering");
}

} // namespace ImGui
