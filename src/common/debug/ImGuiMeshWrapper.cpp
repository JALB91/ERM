#include "erm/debug/ImGuiMeshWrapper.h"
#include "erm/debug/ImGuiMaterialWrapper.h"

#include "erm/rendering/data_structs/Mesh.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowMeshDebugWindow(erm::Game& game, erm::Mesh& mesh, unsigned int meshId)
	{
		static const std::string title = "Mesh ";
		
		ImGui::PushID(static_cast<int>(meshId));
		if (ImGui::CollapsingHeader(mesh.GetName().empty() ? (title + std::to_string(meshId)).c_str() : mesh.GetName().c_str()))
		{
			ImGui::Indent();
			
			ImGui::Text("Vertices: %d", mesh.GetVerticesDataCount());
			ImGui::Text("Indices: %d", mesh.GetIndicesCount());
			ImGui::ShowMaterialDebug(game, mesh);
			
			ImGui::Unindent();
		}
		ImGui::PopID();
	}
	
}