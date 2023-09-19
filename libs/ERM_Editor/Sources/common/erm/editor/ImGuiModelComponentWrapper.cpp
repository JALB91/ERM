#include "erm/debug/ImGuiModelComponentWrapper.h"
#include "erm/debug/ImGuiMeshWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/resources/models/Mesh.h"
#include "erm/resources/models/Model.h"

#include "erm/resources/ResourcesManager.h"

#include "erm/ecs/components/ModelComponent.h"

#include <imgui.h>

namespace ImGui {

void ShowPathOptions(erm::Engine& engine, erm::ecs::ModelComponent& modelComponent)
{
	const std::vector<std::string>& all = engine.GetFileLocator().GetModels();

	erm::Model* model = modelComponent.GetModel();
	std::string currentPath = model ? model->mPath : "";

	if (ImGui::BeginCombo("Path", currentPath.c_str()))
	{
		bool isSelected = currentPath == "";
		ImGui::PushID("NO_SELECTABLE");
		if (ImGui::Selectable("", &isSelected))
		{
			currentPath = "";
			modelComponent.SetModel(nullptr);
		}
		ImGui::PopID();

		for (unsigned int i = 0; i < all.size(); ++i)
		{
			isSelected = currentPath == all[i];
			if (ImGui::Selectable(all[i].c_str(), &isSelected))
			{
				if (currentPath != all[i])
				{
					currentPath = all[i];
					modelComponent.SetModel(engine.GetResourcesManager().GetOrCreateModel(all[i].c_str()));
				}
			}
		}
		ImGui::EndCombo();
	}
}

bool ShowModelComponentDebugWindow(erm::Engine& engine, erm::ecs::ModelComponent& modelComponent)
{
	const bool headerOpen = ImGui::CollapsingHeader("Model");

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("ModelPopup");
	}

	bool shouldRemove = false;

	if (ImGui::BeginPopup("ModelPopup"))
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

		if (erm::Model* model = modelComponent.GetModel())
		{
			u64 vertices = 0;
			u64 indices = 0;
			const std::vector<const erm::Mesh*>& meshes = model->GetMeshes();

			bool showMeshes = ImGui::CollapsingHeader("Meshes");

			ImGui::Indent();
			for (unsigned int i = 0; i < meshes.size(); ++i)
			{
				const erm::Mesh& mesh = *meshes[i];

				if (showMeshes)
				{
//					TODO: Damiano
//					ImGui::ShowMeshDebugWindow(engine, mesh, i);
				}

				vertices += mesh.GetVerticesData().size();
				indices += mesh.GetIndicesData().size();
			}
			ImGui::Unindent();

			ShowPathOptions(engine, modelComponent);
			ImGui::Text("Name: %s", model->mName.c_str());
			ImGui::Text("Vertices: %zu", vertices);
			ImGui::Text("Indices: %zu", indices);

			bool shouldShowBoundingBox = modelComponent.GetShouldShowBoundingBox();
			ImGui::Checkbox("Show Bounding Box", &shouldShowBoundingBox);
			modelComponent.SetShouldShowBoundingBox(shouldShowBoundingBox);
		}
		else
		{
			ShowPathOptions(engine, modelComponent);
		}

		ImGui::Unindent();
	}

	return shouldRemove;
}

} // namespace ImGui
