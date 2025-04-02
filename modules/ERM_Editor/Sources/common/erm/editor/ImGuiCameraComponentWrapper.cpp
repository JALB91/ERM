#include "erm/editor/ImGuiCameraComponentWrapper.h"

#include <erm/ecs/components/CameraComponent.h>

#include <imgui.h>

namespace ImGui {

bool ShowCameraComponentDebugWindow(erm::ecs::CameraComponent& cameraComponent)
{
	const bool headerOpen = ImGui::CollapsingHeader("Camera");

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("CameraPopup");
	}

	bool shouldRemove = false;

	if (ImGui::BeginPopup("CameraPopup"))
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
		ImGui::Indent();

		float movementSpeed = cameraComponent.getMovementSpeed();
		float mouseSensibility = cameraComponent.getMouseSensibility();
		float angleLimit = cameraComponent.getAngleLimit();
		float fov = cameraComponent.getFOV();
		float zNear = cameraComponent.getZNear();
		float zFar = cameraComponent.getZFar();

		ImGui::InputFloat("Movement Speed", &movementSpeed);
		ImGui::InputFloat("Mouse Sensibility", &mouseSensibility);
		ImGui::DragFloat("Angle Limit", &angleLimit, 0.1f, 0.0f, static_cast<float>(M_PI));
		ImGui::DragFloat("FOV", &fov, 1.0f, 0.0f, 180.0f);
		ImGui::DragFloat("Z-Near", &zNear, 1.0f, 0.1f, zFar);
		ImGui::DragFloat("Z-Far", &zFar, 1.0f, zNear, 10000.0f);

		cameraComponent.setMovementSpeed(movementSpeed);
		cameraComponent.setMouseSensibility(mouseSensibility);
		cameraComponent.setAngleLimit(angleLimit);
		cameraComponent.setFOV(fov);
		cameraComponent.setZNear(zNear);
		cameraComponent.setZFar(zFar);

		ImGui::Unindent();
	}

	return shouldRemove;
}

} // namespace ImGui
