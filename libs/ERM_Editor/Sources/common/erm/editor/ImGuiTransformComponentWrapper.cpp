#include "erm/editor/ImGuiTransformComponentWrapper.h"

#include <erm/ecs/components/TransformComponent.h>

#include <imgui.h>

namespace ImGui {

bool ShowTransformComponentDebugWindow(erm::ecs::TransformComponent& transformComponent)
{
	const bool headerOpen = ImGui::CollapsingHeader("Transform");

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("TransformPopup");
	}

	bool shouldRemove = false;

	if (ImGui::BeginPopup("TransformPopup"))
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
		erm::vec3 translation = transformComponent.getTranslation();
		erm::vec3 rotation = transformComponent.getRotation();
		erm::vec3 scale = transformComponent.getScale();

		ImGui::Indent();
		const bool reset = ImGui::Button("Reset");
		ImGui::InputFloat3("Translation", &translation.x);
		ImGui::SliderFloat3("Rotation", &rotation.x, -static_cast<float>(M_PI), static_cast<float>(M_PI));
		ImGui::InputFloat3("Scale", &scale.x);
		ImGui::Unindent();

		if (reset)
		{
			translation = erm::vec3(0.0f);
			rotation = erm::vec3(0.0f);
			scale = erm::vec3(1.0f);
		}

		transformComponent.setTranslation(std::move(translation));
		transformComponent.setRotation(std::move(rotation));
		transformComponent.setScale(std::move(scale));
	}

	return shouldRemove;
}

} // namespace ImGui
