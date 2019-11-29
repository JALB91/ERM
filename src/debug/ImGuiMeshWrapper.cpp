#include "debug/ImGuiMeshWrapper.h"
#include "debug/ImGuiMaterialWrapper.h"

#include "rendering/Mesh.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowMeshDebugWindow(erm::Mesh& mesh, unsigned int meshId)
	{
		static const std::string title = "Mesh ";
		
		ImGui::PushID(meshId);
		if (ImGui::CollapsingHeader(mesh.GetName().empty() ? (title + std::to_string(meshId)).c_str() : mesh.GetName().c_str()))
		{
			ImGui::Indent();
			
			ImGui::Text("Vertices: %d", mesh.GetVerticesDataCount());
			ImGui::Text("Indices: %d", mesh.GetIndicesCount());
			ImGui::ShowMaterialDebug(mesh);
			
			ImGui::Unindent();
		}
		ImGui::PopID();
	}
	
}
