#include "debug/ImGuiMaterialWrapper.h"

#include "managers/ResourcesManager.h"

#include "rendering/Material.h"
#include "rendering/Mesh.h"

#include <imgui.h>

#include <set>

namespace ImGui {
	
	void ShowMaterialDebug(erm::Mesh& mesh)
	{
		if (ImGui::CollapsingHeader("Material"))
		{
			erm::Material* material = mesh.GetMaterial();
			
			ImGui::Indent();
			
			static erm::ResourcesManager::Materials& all = erm::ResourcesManager::GetLoadedMaterials();
			std::string currentPath = material ? material->mPath : "";
			std::string currentName = material ? material->mName : "";
			std::set<std::string> displayedPaths;
			
			if (ImGui::BeginCombo("Path", currentPath.c_str()))
			{
				bool isSelected = currentPath == "";
				if (ImGui::Selectable("", &isSelected))
				{
					currentPath = "";
					currentName = "";
					mesh.SetMaterial(nullptr);
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
				bool isSelected = currentName == "";
				if (ImGui::Selectable("", &isSelected))
				{
					currentName = "";
					mesh.SetMaterial(nullptr);
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
			
			if (erm::Material* material = mesh.GetMaterial())
			{
				ImGui::SliderFloat3("Ambient", &material->mAmbient.x, 0.0f, 1.0f);
				ImGui::SliderFloat3("Diffuse", &material->mDiffuse.x, 0.0f, 1.0f);
				ImGui::SliderFloat3("Specular", &material->mSpecular.x, 0.0f, 1.0f);
				ImGui::SliderFloat("Shininess", &material->mShininess, 0.0f, 1000.0f);
			}
			
			ImGui::Unindent();
		}
	}
	
}
