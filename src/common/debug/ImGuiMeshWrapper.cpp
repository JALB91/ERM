#include "erm/debug/ImGuiMeshWrapper.h"
#include "erm/debug/ImGuiMaterialWrapper.h"
#include "erm/debug/ImGuiPBMaterialWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/shaders/ShaderProgram.h"

#include <imgui.h>

namespace ImGui {

	void ShowShaderPathOptions(erm::Engine& engine, erm::Mesh& mesh)
	{
		const std::vector<std::string>& all = engine.GetFileLocator().GetShaderPrograms();

		erm::ShaderProgram* shader = mesh.GetRenderConfigs().mShaderProgram;
		std::string currentPath = shader ? shader->mPath : "";

		if (ImGui::BeginCombo("Shader", currentPath.c_str()))
		{
			bool isSelected = currentPath == "";
			if (ImGui::Selectable("", &isSelected))
			{
				currentPath = "";
				mesh.GetRenderConfigs().mShaderProgram = nullptr;
			}
			for (unsigned int i = 0; i < all.size(); ++i)
			{
				isSelected = currentPath == all[i];
				if (ImGui::Selectable(all[i].c_str(), &isSelected))
				{
					if (currentPath != all[i])
					{
						currentPath = all[i];
						mesh.GetRenderConfigs().mShaderProgram = engine.GetResourcesManager().GetOrCreateShaderProgram(all[i].c_str());
					}
				}
			}
			ImGui::EndCombo();
		}
	}

	void ShowTexturePathOptions(erm::Engine& engine, erm::Texture** texture, const char* name)
	{
		const std::vector<std::string>& all = engine.GetFileLocator().GetTextures();

		std::string currentPath = *texture ? (*texture)->mPath : "";

		if (ImGui::BeginCombo(name, currentPath.c_str()))
		{
			bool isSelected = currentPath == "";
			if (ImGui::Selectable("", &isSelected))
			{
				currentPath = "";
				*texture = nullptr;
			}
			for (unsigned int i = 0; i < all.size(); ++i)
			{
				isSelected = currentPath == all[i];
				if (ImGui::Selectable(all[i].c_str(), &isSelected))
				{
					if (currentPath != all[i])
					{
						currentPath = all[i];
						*texture = engine.GetResourcesManager().GetOrCreateTexture(all[i].c_str());
					}
				}
			}
			ImGui::EndCombo();
		}
	}

	void ShowMeshDebugWindow(erm::Engine& engine, erm::Mesh& mesh, unsigned int meshId)
	{
		static const std::string title = "Mesh ";

		ImGui::PushID(static_cast<int>(meshId));
		if (ImGui::CollapsingHeader(mesh.GetName().empty() ? (title + std::to_string(meshId)).c_str() : mesh.GetName().c_str()))
		{
			ImGui::Indent();

			ImGui::Text("Vertices: %u", mesh.GetVerticesData().size());
			ImGui::Text("Indices: %u", mesh.GetIndicesData().size());
			ImGui::ShowPBMaterialDebug(engine, mesh);
			ImGui::ShowMaterialDebug(engine, mesh);

			ShowShaderPathOptions(engine, mesh);
			ShowTexturePathOptions(engine, &mesh.GetRenderConfigs().mDiffuseMap, "Diffuse Map");
			ShowTexturePathOptions(engine, &mesh.GetRenderConfigs().mNormalMap, "Normal Map");
			ShowTexturePathOptions(engine, &mesh.GetRenderConfigs().mSpecularMap, "Specular Map");

			ImGui::Unindent();
		}
		ImGui::PopID();
	}

} // namespace ImGui
