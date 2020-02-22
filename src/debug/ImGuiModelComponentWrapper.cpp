#include "debug/ImGuiModelComponentWrapper.h"
#include "debug/ImGuiMeshWrapper.h"

#include "game/Game.h"

#include "rendering/Model.h"
#include "rendering/Mesh.h"

#include "managers/ResourcesManager.h"

#include "ecs/components/ModelComponent.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowPathOptions(erm::Game& game, erm::ecs::ModelComponent& modelComponent)
	{
		const std::vector<std::string>& all = game.GetFileLocator().GetModels();
		
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
				bool isSelected = currentPath == all[i];
				if (ImGui::Selectable(all[i].c_str(), &isSelected))
				{
					if (currentPath != all[i])
					{
						currentPath = all[i];
						modelComponent.SetModel(game.GetResourcesManager().GetOrCreateModel(all[i].c_str()));
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	
	bool ShowModelComponentDebugWindow(erm::Game& game, erm::ecs::ModelComponent& modelComponent)
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
						ImGui::ShowMeshDebugWindow(game, mesh, i);
					}
					
					vertices += mesh.GetVerticesDataCount();
					indices += mesh.GetIndicesCount();
				}
				ImGui::Unindent();
				
				ShowPathOptions(game, modelComponent);
				ImGui::Text("Name: %s", model->GetName().c_str());
				ImGui::Text("Vertices: %d", vertices);
				ImGui::Text("Indices: %d", indices);
				
				bool shouldShowBoundingBox = modelComponent.GetShouldShowBoundingBox();
				ImGui::Checkbox("Show Bounding Box", &shouldShowBoundingBox);
				modelComponent.SetShouldShowBoundingBox(shouldShowBoundingBox);
			}
			else
			{
				ShowPathOptions(game, modelComponent);
			}
			
			ImGui::Unindent();
		}
		
		return shouldRemove;
	}
	
}
