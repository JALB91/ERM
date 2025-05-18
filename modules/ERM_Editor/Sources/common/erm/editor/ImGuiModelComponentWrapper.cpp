#include "erm/editor/ImGuiModelComponentWrapper.h"
#include "erm/editor/ImGuiMeshWrapper.h"

#include <erm/assets/models/Mesh.h>
#include <erm/assets/models/Model.h>
#include <erm/assets/AssetsManager.h>
#include <erm/ecs/components/ModelComponent.h>
#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>

#include <imgui.h>

namespace ImGui {

void ShowPathOptions(erm::ecs::ModelComponent& /*modelComponent*/)
{
	//const std::vector<std::string>& all = engine.getFileLocator().getModels();

	/*erm::Model* model = modelComponent.getModel();
	std::string currentPath = model ? model->mPath : "";

	if (ImGui::BeginCombo("Path", currentPath.c_str()))
	{
		bool isSelected = currentPath == "";
		ImGui::PushID("NO_SELECTABLE");
		if (ImGui::Selectable("", &isSelected))
		{
			currentPath = "";
			modelComponent.setModel(nullptr);
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
					modelComponent.setModel(engine.GetResourcesManager().GetOrCreateModel(all[i].c_str()));
				}
			}
		}
		ImGui::EndCombo();
	}*/
}

bool ShowModelComponentDebugWindow(erm::ecs::ModelComponent& /*modelComponent*/)
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

//		if (erm::Model* model = modelComponent.getModel())
//		{
//			erm::u64 vertices = 0;
//			erm::u64 indices = 0;
//			const std::vector<const erm::Mesh*>& meshes = model->getMeshes();
//
//			bool showMeshes = ImGui::CollapsingHeader("Meshes");
//
//			ImGui::Indent();
//			for (unsigned int i = 0; i < meshes.size(); ++i)
//			{
//				const erm::Mesh& mesh = *meshes[i];
//
//				if (showMeshes)
//				{
////					TODO: Damiano
////					ImGui::ShowMeshDebugWindow(engine, mesh, i);
//				}
//
//				vertices += mesh.getVerticesData().size();
//				indices += mesh.getIndicesData().size();
//			}
//			ImGui::Unindent();
//
//			ShowPathOptions(engine, modelComponent);
//			ImGui::Text("Name: %s", model->mName.c_str());
//			ImGui::Text("Vertices: %zu", vertices);
//			ImGui::Text("Indices: %zu", indices);
//
//			bool shouldShowBoundingBox = modelComponent.getShouldShowBoundingBox();
//			ImGui::Checkbox("Show Bounding Box", &shouldShowBoundingBox);
//			modelComponent.setShouldShowBoundingBox(shouldShowBoundingBox);
//		}
//		else
//		{
//			ShowPathOptions(engine, modelComponent);
//		}

		ImGui::Unindent();
	}

	return shouldRemove;
}

} // namespace ImGui
