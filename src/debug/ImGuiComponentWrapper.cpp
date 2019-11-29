#include "debug/ImGuiComponentWrapper.h"
#include "debug/ImGuiTransformComponentWrapper.h"
#include "debug/ImGuiModelComponentWrapper.h"
#include "debug/ImGuiCameraComponentWrapper.h"

#include "ec/IComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/components/ModelComponent.h"
#include "ec/components/CameraComponent.h"

namespace ImGui {
	
	void ShowComponentDebugWindow(erm::IComponent* component)
	{
		if (!component) return;
		
		if (erm::TransformComponent* transformComponent = dynamic_cast<erm::TransformComponent*>(component))
		{
			ShowTransformComponentDebugWindow(*transformComponent);
		}
		else if (erm::ModelComponent* modelComponent = dynamic_cast<erm::ModelComponent*>(component))
		{
			ShowModelComponentDebugWindow(*modelComponent);
		}
		else if (erm::CameraComponent* cameraComponent = dynamic_cast<erm::CameraComponent*>(component))
		{
			ShowCameraComponentDebugWindow(*cameraComponent);
		}
	}
	
}
