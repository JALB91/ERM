#include "debug/ImGuiTransformComponentWrapper.h"

#include "ecs/components/TransformComponent.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowTransformComponentDebugWindow(erm::ecs::TransformComponent& transformComponent)
	{
		if (ImGui::CollapsingHeader("Transform"))
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
	}
	
}
