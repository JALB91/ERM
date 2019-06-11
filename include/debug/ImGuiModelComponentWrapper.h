#pragma once

#include "rendering/Model.h"
#include "rendering/Mesh.h"

#include "ec/components/ModelComponent.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowModelComponentDebugWindow(erm::ModelComponent& modelComponent)
	{
		if (ImGui::CollapsingHeader("Model"))
		{
			ImGui::Indent();
			
			ImGui::Text("Name: %s", modelComponent.GetModel().GetName().c_str());
			ImGui::Text("Path: %s", modelComponent.GetModel().GetPath().c_str());
			
			int vertices = 0;
			int indices = 0;
			for (const erm::Mesh& mesh: modelComponent.GetModel().GetMeshes())
			{
				vertices += mesh.GetVerticesDataCount();
				indices += mesh.GetIndicesCount();
			}
			ImGui::Text("Vertices: %d", vertices);
			ImGui::Text("Indices: %d", indices);
			
			bool shouldShowBoundingBox = modelComponent.ShouldShowBoundingBox();
			ImGui::Checkbox("Show Bounding Box", &shouldShowBoundingBox);
			modelComponent.SetShouldShowBoundingBox(shouldShowBoundingBox);
			
			ImGui::Unindent();
		}
	}
	
}
