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
			erm::math::vec3& translation = transformComponent.mTranslation;
			erm::math::vec3& rotation = transformComponent.mRotation;
			erm::math::vec3& scale = transformComponent.mScale;
			
			ImGui::Indent();
			bool reset = ImGui::Button("Reset");
			ImGui::InputFloat3("Translation", &translation.x, 1);
			ImGui::SliderFloat3("Rotation", &rotation.x, -static_cast<float>(M_PI), static_cast<float>(M_PI));
			ImGui::InputFloat3("Scale", &scale.x, 1);
			ImGui::Unindent();
			
			if (reset)
			{
				translation = erm::math::vec3(0.0f);
				rotation = erm::math::vec3(0.0f);
				scale = erm::math::vec3(1.0f);
			}
		}
		
		return shouldRemove;
	}
	
}
