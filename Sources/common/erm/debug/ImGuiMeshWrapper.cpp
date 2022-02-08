#include "erm/debug/ImGuiMeshWrapper.h"
#include "erm/debug/ImGuiMaterialWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

#include <imgui.h>

namespace ImGui {

void ShowShaderPathOptions(erm::Engine& engine, erm::Mesh& mesh)
{
	const std::vector<std::string>& all = engine.GetFileLocator().GetShaderPrograms();

	erm::PipelineConfigs& configs = mesh.GetPipelineConfigs();
	erm::IShaderProgram* shader = configs.mShaderProgram;
	std::string currentPath = shader ? shader->mPath : "";

	if (ImGui::BeginCombo("Shader", currentPath.c_str()))
	{
		bool isSelected = currentPath == "";
		ImGui::PushID("NO_SELECTABLE");
		if (ImGui::Selectable("", &isSelected))
		{
			currentPath = "";
			configs.mShaderProgram = nullptr;
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
					configs.mShaderProgram = engine.GetResourcesManager().GetOrCreateShaderProgram(all[i].c_str());
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
		ImGui::PushID("NO_SELECTABLE");
		if (ImGui::Selectable("", &isSelected))
		{
			currentPath = "";
			*texture = nullptr;
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

		ImGui::Text("Vertices: %zu", mesh.GetVerticesData().size());
		ImGui::Text("Indices: %zu", mesh.GetIndicesData().size());
		ImGui::ShowMaterialDebug(engine, mesh);

		erm::PipelineConfigs& configs = mesh.GetPipelineConfigs();

		ShowShaderPathOptions(engine, mesh);
		ShowTexturePathOptions(engine, &configs.mTexturesMaps[erm::TextureType::DIFFUSE], "Diffuse Map");
		ShowTexturePathOptions(engine, &configs.mTexturesMaps[erm::TextureType::NORMAL], "Normal Map");
		ShowTexturePathOptions(engine, &configs.mTexturesMaps[erm::TextureType::SPECULAR], "Specular Map");

		ImGui::Unindent();
	}
	ImGui::PopID();
}

} // namespace ImGui
