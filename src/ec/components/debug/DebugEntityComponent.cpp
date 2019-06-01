#include "ec/components/debug/DebugEntityComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/Entity.h"

#include <imgui.h>

#include <glm/glm.hpp>

#include <cstdio>

namespace erm {
	
	DebugEntityComponent::DebugEntityComponent(Entity& entity)
		: IComponent(entity)
		, mTransform(entity.RequireComponent<TransformComponent>())
	{}
	
	DebugEntityComponent::~DebugEntityComponent()
	{}
	
	void DebugEntityComponent::OnImGuiRender()
	{
		char buffer [64];
		std::sprintf(buffer, "Entity %d", mEntity.GetEntityID());
		
		if (ImGui::Begin(buffer))
		{
			glm::vec3 translation = mTransform.GetTranslation();
			glm::vec3 rotation = mTransform.GetRotation();
			glm::vec3 scale = mTransform.GetScale();
			
			bool reset = ImGui::Button("Reset");
			ImGui::InputFloat3("Translation", &translation.x, 1);
			ImGui::SliderFloat3("Rotation", &rotation.x, -static_cast<float>(M_PI), static_cast<float>(M_PI));
			ImGui::InputFloat3("Scale", &scale.x, 1);
			
//			ImGui::Text("Drag Mode");
//			ImGui::RadioButton("None", &mDragMode, 0); ImGui::SameLine();
//			ImGui::RadioButton("Translate", &mDragMode, 1); ImGui::SameLine();
//			ImGui::RadioButton("Rotate", &mDragMode, 2);
			
			if (reset)
			{
				mTransform.SetTranslation(glm::vec3(0.0f));
				mTransform.SetRotation(glm::vec3(0.0f));
				mTransform.SetScale(glm::vec3(1.0f));
			}
			else
			{
				mTransform.SetTranslation(translation);
				mTransform.SetRotation(rotation);
				mTransform.SetScale(scale);
			}
		}
		ImGui::End();
	}
	
}
