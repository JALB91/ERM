#include "erm/debug/ImGuiMaterialWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/data_structs/Mesh.h"

#include "erm/utils/Utils.h"

#include <imgui.h>

#include <set>

namespace ImGui {

	void ShowMaterialDebug(erm::Engine& engine, erm::Mesh& mesh)
	{
		if (ImGui::CollapsingHeader("Material"))
		{
			erm::Material* material = mesh.GetRenderConfigs().mMaterial ? mesh.GetRenderConfigs().mMaterial : &erm::Material::DEFAULT;

			ImGui::Indent();

			static erm::Materials& all = engine.GetResourcesManager().GetMaterials();
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
					mesh.GetRenderConfigs().mMaterial = &erm::Material::DEFAULT;
				}

				for (unsigned int i = 0; i < all.size(); ++i)
				{
					if (std::find(displayedPaths.begin(), displayedPaths.end(), all[i]->mPath) != displayedPaths.end())
					{
						continue;
					}

					displayedPaths.insert(all[i]->mPath);

					isSelected = currentPath == all[i]->mPath;
					if (ImGui::Selectable(all[i]->mPath.c_str(), &isSelected))
					{
						if (currentPath != all[i]->mPath)
						{
							currentPath = all[i]->mPath;
							currentName = all[i]->mName;
							mesh.GetRenderConfigs().mMaterial = all[i].get();
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
					mesh.GetRenderConfigs().mMaterial = &erm::Material::DEFAULT;
				}

				for (unsigned int i = 0; i < all.size(); ++i)
				{
					if (currentPath == all[i]->mPath)
					{
						isSelected = currentName == all[i]->mName;
						if (ImGui::Selectable(all[i]->mName.c_str(), &isSelected))
						{
							if (currentName != all[i]->mName)
							{
								currentName = all[i]->mName;
								mesh.GetRenderConfigs().mMaterial = all[i].get();
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

			ImGui::Unindent();
		}
	}

} // namespace ImGui
