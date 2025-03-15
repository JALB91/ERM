#include "erm/editor/ImGuiLightComponentWrapper.h"

#include <erm/ecs/components/LightComponent.h>

#include <imgui.h>

namespace ImGui {

bool ShowLightComponentDebugWindow(erm::ecs::LightComponent& lightComponent)
{
	const bool headerOpen = ImGui::CollapsingHeader("Light");

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("LightPopup");
	}

	bool shouldRemove = false;

	if (ImGui::BeginPopup("LightPopup"))
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

		ImGui::InputFloat3("Ambient", &lightComponent.mAmbient[0]);
		ImGui::InputFloat3("Diffuse", &lightComponent.mDiffuse[0]);
		ImGui::InputFloat3("Specular", &lightComponent.mSpecular[0]);

		ImGui::Unindent();
	}

	return shouldRemove;
}

} // namespace ImGui
