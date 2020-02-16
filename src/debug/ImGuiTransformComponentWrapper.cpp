#include "debug/ImGuiTransformComponentWrapper.h"

#include "ecs/components/TransformComponent.h"

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
			erm::math::vec3 translation = transformComponent.GetTranslation();
			erm::math::vec3 rotation = transformComponent.GetRotation();
			erm::math::vec3 scale = transformComponent.GetScale();
			
			ImGui::Indent();
			bool reset = ImGui::Button("Reset");
			ImGui::InputFloat3("Translation", &translation.x, 1);
			ImGui::SliderFloat3("Rotation", &rotation.x, -static_cast<float>(M_PI), static_cast<float>(M_PI));
			ImGui::InputFloat3("Scale", &scale.x, 1);
			ImGui::Unindent();
			
			if (reset)
			{
				transformComponent.SetTranslation(erm::math::vec3(0.0f));
				transformComponent.SetRotation(erm::math::vec3(0.0f));
				transformComponent.SetScale(erm::math::vec3(1.0f));
			}
			else
			{
				transformComponent.SetTranslation(translation);
				transformComponent.SetRotation(rotation);
				transformComponent.SetScale(scale);
			}
		}
		
		return shouldRemove;
	}
	
}
