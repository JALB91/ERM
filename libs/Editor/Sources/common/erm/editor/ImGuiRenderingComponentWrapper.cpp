#include "erm/debug/ImGuiRenderingComponentWrapper.h"

#include "erm/ecs/components/RenderingComponent.h"

#include <imgui.h>

namespace ImGui {

bool ShowRenderingComponentDebug(erm::ecs::RenderingComponent& renderingComponent)
{
	ERM_UNUSED(renderingComponent);

	const bool headerOpen = ImGui::CollapsingHeader("Rendering");

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("RenderingComponentPopup");
	}

	bool shouldRemove = false;

	if (ImGui::BeginPopup("RenderingComponentPopup"))
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

#ifdef ERM_RAY_TRACING_ENABLED
		bool useRayTracing = renderingComponent.GetUseRayTracing();
		if (ImGui::Checkbox("Use RTX", &useRayTracing))
			renderingComponent.SetUseRayTracing(useRayTracing);
#endif

		ImGui::Unindent();
	}

	return shouldRemove;
}

} // namespace ImGui
