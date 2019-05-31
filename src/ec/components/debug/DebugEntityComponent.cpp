#include "ec/components/debug/DebugEntityComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/Entity.h"

#include <imgui.h>

#include <glm/glm.hpp>

#include <string>

namespace erm {
	
	DebugEntityComponent::DebugEntityComponent(Entity& entity)
		: IComponent(entity)
		, mTransformComponent(entity.RequireComponent<TransformComponent>())
	{}
	
	DebugEntityComponent::~DebugEntityComponent()
	{}
	
	void DebugEntityComponent::OnImGuiRender()
	{
		char buffer [64];
		std::sprintf(buffer, "Entity %d", mEntity.GetEntityID());
		ImGui::Begin(buffer);
		
		glm::vec3 translation = mTransformComponent.GetTranslation();
		glm::vec3 rotation = mTransformComponent.GetRotation();
		glm::vec3 scale = mTransformComponent.GetScale();
		
		bool reset = ImGui::Button("Reset");
		ImGui::InputFloat3("Translation", &translation.x, 1);
		ImGui::SliderFloat3("Rotation", &rotation.x, -static_cast<float>(M_PI), static_cast<float>(M_PI));
		ImGui::InputFloat3("Scale", &scale.x, 1);
		
//		ImGui::Text("Drag Mode");
//		ImGui::RadioButton("None", &mDragMode, 0); ImGui::SameLine();
//		ImGui::RadioButton("Translate", &mDragMode, 1); ImGui::SameLine();
//		ImGui::RadioButton("Rotate", &mDragMode, 2);
		
		if (reset)
		{
			mTransformComponent.SetTranslation(glm::vec3(0.0f));
			mTransformComponent.SetRotation(glm::vec3(0.0f));
			mTransformComponent.SetScale(glm::vec3(1.0f));
		}
		else
		{
			mTransformComponent.SetTranslation(translation);
			mTransformComponent.SetRotation(rotation);
			mTransformComponent.SetScale(scale);
		}
		
		ImGui::End();
	}
	
}
