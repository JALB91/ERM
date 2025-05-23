#include "erm/editor/ImGuiComponentWrapper.h"
#include "erm/editor/ImGuiAnimationComponentWrapper.h"
#include "erm/editor/ImGuiCameraComponentWrapper.h"
#include "erm/editor/ImGuiLightComponentWrapper.h"
#include "erm/editor/ImGuiModelComponentWrapper.h"
#include "erm/editor/ImGuiRenderingComponentWrapper.h"
#include "erm/editor/ImGuiSkeletonComponentWrapper.h"
#include "erm/editor/ImGuiTransformComponentWrapper.h"

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
#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/window/Window.h>

#include <imgui.h>

namespace ImGui {

template<typename T>
void ShowComponentDebugWindow(erm::ecs::EntityId entity, const std::function<bool(T&)>& callback, const char* name)
{
	auto ecs = erm::ObjectRegistry::get<erm::ecs::ECS>();
	if (T* component = ecs->getSystem<typename T::SYSTEM_TYPE>()->getComponent(entity))
	{
		ImGui::PushID(name);
		if (callback(*component))
		{
			ecs->getSystem<typename T::SYSTEM_TYPE>()->removeComponent(entity);
		}
		ImGui::PopID();
	}
	else if (ImGui::BeginPopup("Components"))
	{
		if (ImGui::Button(name))
		{
			ecs->addComponent<T>(entity);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ShowComponentDebugWindow(erm::ecs::EntityId entity)
{
	if (!entity.isValid())
		return;

	if (ImGui::Button("Add Component..."))
	{
		ImGui::OpenPopup("Components");
	}

	ImGui::Separator();

	ShowComponentDebugWindow<erm::ecs::TransformComponent>(
		entity,
		[](erm::ecs::TransformComponent& component) {
			return ShowTransformComponentDebugWindow(component);
		},
		"Transform");
	ShowComponentDebugWindow<erm::ecs::ModelComponent>(
		entity,
		[](erm::ecs::ModelComponent& component) {
			return ShowModelComponentDebugWindow(component);
		},
		"Model");
	ShowComponentDebugWindow<erm::ecs::CameraComponent>(
		entity,
		[](erm::ecs::CameraComponent& component) {
			return ShowCameraComponentDebugWindow(component);
		},
		"Camera");
	ShowComponentDebugWindow<erm::ecs::LightComponent>(
		entity,
		[](erm::ecs::LightComponent& component) {
			return ShowLightComponentDebugWindow(component);
		},
		"Light");
	ShowComponentDebugWindow<erm::ecs::SkeletonComponent>(
		entity,
		[](erm::ecs::SkeletonComponent& component) {
			return ShowSkeletonComponentDebugWindow(component);
		},
		"Skeleton");
	ShowComponentDebugWindow<erm::ecs::AnimationComponent>(
		entity,
		[](erm::ecs::AnimationComponent& component) {
			return ShowAnimationComponentDebugWindow(component);
		},
		"Animation");
	ShowComponentDebugWindow<erm::ecs::RenderingComponent>(
		entity,
		[](erm::ecs::RenderingComponent& component) {
			return ShowRenderingComponentDebug(component);
		},
		"Rendering");
}

} // namespace ImGui
