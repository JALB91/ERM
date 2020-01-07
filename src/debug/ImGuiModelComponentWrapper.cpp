#include "debug/ImGuiModelComponentWrapper.h"
#include "debug/ImGuiMeshWrapper.h"

#include "rendering/Model.h"
#include "rendering/Mesh.h"

#include "managers/ResourcesManager.h"

#include "ec/components/ModelComponent.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowPathOptions(erm::ModelComponent& modelComponent)
	{
		static erm::ResourcesManager::Models& all = erm::ResourcesManager::GetLoadedModels();
		
		erm::Model* model = modelComponent.GetModel();
		std::string currentPath = model ? model->GetPath() : "";
		
		if (ImGui::BeginCombo("Path", currentPath.c_str()))
		{
			bool isSelected = currentPath == "";
			if (ImGui::Selectable("", &isSelected))
			{
				currentPath = "";
				modelComponent.SetModel(nullptr);
			}
			
			for (unsigned int i = 0; i < all.size(); ++i)
			{
				bool isSelected = currentPath == all[i]->GetPath();
				if (ImGui::Selectable(all[i]->GetPath().c_str(), &isSelected))
				{
					if (currentPath != all[i]->GetPath())
					{
						currentPath = all[i]->GetPath();
						modelComponent.SetModel(all[i].get());
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	
	void ShowModelComponentDebugWindow(erm::ModelComponent& modelComponent)
	{
		if (ImGui::CollapsingHeader("Model"))
		{
			ImGui::Indent();
			
			if (erm::Model* model = modelComponent.GetModel())
			{
				int vertices = 0;
				int indices = 0;
				std::vector<erm::Mesh>& meshes = model->GetMeshes();
				
				bool showMeshes = ImGui::CollapsingHeader("Meshes");
				
				ImGui::Indent();
				for (unsigned int i = 0; i < meshes.size(); ++i)
				{
					erm::Mesh& mesh = meshes[i];
					
					if (showMeshes)
					{
						ImGui::ShowMeshDebugWindow(mesh, i);
					}
					
					vertices += mesh.GetVerticesDataCount();
					indices += mesh.GetIndicesCount();
				}
				ImGui::Unindent();
				
				ShowPathOptions(modelComponent);
				ImGui::Text("Name: %s", model->GetName().c_str());
				ImGui::Text("Vertices: %d", vertices);
				ImGui::Text("Indices: %d", indices);
				
				bool shouldShowBoundingBox = modelComponent.ShouldShowBoundingBox();
				ImGui::Checkbox("Show Bounding Box", &shouldShowBoundingBox);
				modelComponent.SetShouldShowBoundingBox(shouldShowBoundingBox);
			}
			else
			{
				ShowPathOptions(modelComponent);
			}
			
			ImGui::Unindent();
		}
	}
	
}
