#pragma once

#include "ec/components/CameraComponent.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowCameraComponentDebugWindow(erm::CameraComponent& cameraComponent)
	{
		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Indent();
			
			float movementSpeed = cameraComponent.GetMovementSpeed();
			float mouseSensibility = cameraComponent.GetMouseSensibility();
			float angleLimit = cameraComponent.GetAngleLimit();
			float fov = cameraComponent.GetFOV();
			float zNear = cameraComponent.GetZNear();
			float zFar = cameraComponent.GetZFar();
			
			ImGui::InputFloat("Movement Speed", &movementSpeed);
			ImGui::InputFloat("Mouse Sensibility", &mouseSensibility);
			ImGui::DragFloat("Angle Limit", &angleLimit, 0.1f, 0.0f, static_cast<float>(M_PI));
			ImGui::DragFloat("FOV", &fov, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat("Z-Near", &zNear, 1.0f, 0.1f, zFar);
			ImGui::DragFloat("Z-Far", &zFar, 1.0f, zNear, 10000.0f);
			
			cameraComponent.SetMovementSpeed(movementSpeed);
			cameraComponent.SetMouseSensibility(mouseSensibility);
			cameraComponent.SetAngleLimit(angleLimit);
			cameraComponent.SetFOV(fov);
			cameraComponent.SetZNear(zNear);
			cameraComponent.SetZFar(zFar);
			
			ImGui::Unindent();
		}
	}
	
}
