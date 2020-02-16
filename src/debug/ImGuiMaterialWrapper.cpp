#include "debug/ImGuiMaterialWrapper.h"

#include "game/Game.h"

#include "managers/ResourcesManager.h"

#include "rendering/Material.h"
#include "rendering/Mesh.h"

#include <imgui.h>

#include <set>

namespace ImGui {
	
	void ShowPathOptions(erm::Material& material)
	{
		static erm::Shaders& all = erm::Game::GetInstance().GetResourcesManager().GetLoadedShaderPrograms();
		
		erm::ShaderProgram* shader = material.mShaderProgram;
		std::string currentPath = shader ? shader->GetPath() : "";
		
		if (ImGui::BeginCombo("Shader Path", currentPath.c_str()))
		{
			bool isSelected = currentPath == "";
			if (ImGui::Selectable("", &isSelected))
			{
				currentPath = "";
				material.mShaderProgram = nullptr;
			}
			
			for (unsigned int i = 0; i < all.size(); ++i)
			{
				bool isSelected = currentPath == all[i]->GetPath();
				if (ImGui::Selectable(all[i]->GetPath().c_str(), &isSelected))
				{
					if (currentPath != all[i]->GetPath())
					{
						currentPath = all[i]->GetPath();
						material.mShaderProgram = all[i].get();
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	
	void ShowMaterialDebug(erm::Mesh& mesh)
	{
		if (ImGui::CollapsingHeader("Material"))
		{
			erm::Material* material = mesh.GetMaterial() ? mesh.GetMaterial() : &erm::Material::DEFAULT;
			
			ImGui::Indent();
			
			static erm::Materials& all = erm::Game::GetInstance().GetResourcesManager().GetLoadedMaterials();
			std::string currentPath = material->mPath;
			std::string currentName = material->mName;
			std::set<std::string> displayedPaths;
			
			if (ImGui::BeginCombo("Material Path", currentPath.c_str()))
			{
				bool isSelected = currentPath == "Default";
				if (ImGui::Selectable("Default", &isSelected))
				{
					currentPath = "Default";
					currentName = "Default";
					mesh.SetMaterial(&erm::Material::DEFAULT);
				}
				
				for (unsigned int i = 0; i < all.size(); ++i)
				{
					if (std::find(displayedPaths.begin(), displayedPaths.end(), all[i]->mPath) != displayedPaths.end())
					{
						continue;
					}
					
					displayedPaths.insert(all[i]->mPath);
					
					bool isSelected = currentPath == all[i]->mPath;
					if (ImGui::Selectable(all[i]->mPath.c_str(), &isSelected))
					{
						if (currentPath != all[i]->mPath)
						{
							currentPath = all[i]->mPath;
							currentName = all[i]->mName;
							mesh.SetMaterial(all[i].get());
						}
					}
				}
				ImGui::EndCombo();
			}
			
			if (ImGui::BeginCombo("Name", currentName.c_str()))
			{
				bool isSelected = currentName == "Default";
				if (ImGui::Selectable("Default", &isSelected))
				{
					currentName = "Default";
					mesh.SetMaterial(&erm::Material::DEFAULT);
				}
				
				for (unsigned int i = 0; i < all.size(); ++i)
				{
					if (currentPath == all[i]->mPath)
					{
						bool isSelected = currentName == all[i]->mName;
						if (ImGui::Selectable(all[i]->mName.c_str(), &isSelected))
						{
							if (currentName != all[i]->mName)
							{
								currentName = all[i]->mName;
								mesh.SetMaterial(all[i].get());
							}
						}
					}
				}
				ImGui::EndCombo();
			}
			
			ImGui::SliderFloat3("Ambient", &material->mAmbient.x, 0.0f, 1.0f);
			ImGui::SliderFloat3("Diffuse", &material->mDiffuse.x, 0.0f, 1.0f);
			ImGui::SliderFloat3("Specular", &material->mSpecular.x, 0.0f, 1.0f);
			ImGui::SliderFloat("Shininess", &material->mShininess, 0.0f, 1000.0f);
			
			ShowPathOptions(*material);
			
			ImGui::Unindent();
		}
	}
	
}
