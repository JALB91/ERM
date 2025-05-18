#include "erm/editor/ImGuiMaterialWrapper.h"

#include <erm/assets/AssetsManager.h>
#include <erm/assets/models/Mesh.h>
#include <erm/assets/materials/Material.h>
#include <erm/assets/materials/PBMaterial.h>
#include <erm/engine/Engine.h>
#include <erm/rendering/data_structs/BindingConfigs.h>
#include <erm/utils/Utils.h>

#include <imgui.h>

#include <set>

namespace ImGui {

void ShowLegacyMaterialDebug(erm::BindingConfigs& configs);
void ShowPBMaterialDebug(erm::BindingConfigs& configs);

template<typename T, typename MaterialType>
void ShowPathOptions(erm::BindingConfigs& /*configs*/, const T& /*all*/)
{
	/*MaterialType* material = static_cast<MaterialType*>(configs.mMaterial.mData);

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
			configs.mMaterial.mData = &erm::Material::DEFAULT;
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
					configs.mMaterial.mData = all[i].get();
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
			configs.mMaterial.mData = &MaterialType::DEFAULT;
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
						configs.mMaterial.mData = all[i].get();
					}
				}
			}
		}
		ImGui::EndCombo();
	}*/
}

void ShowMaterialDebug(erm::Mesh& /*mesh*/)
{
	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::Indent();

//		TODO: Damiano
//		erm::BindingConfigs& configs = mesh.GetPipelineConfigs();
		/*erm::BindingConfigs configs;
		erm::MaterialHandle& material = configs.mMaterial;

		const bool isLegacy = material.mType == erm::MaterialType::LEGACY;
		const bool isPbr = material.mType == erm::MaterialType::PBR;

		const bool switchLegacy = ImGui::RadioButton("Legacy", isLegacy);
		ImGui::SameLine();
		const bool switchPbr = ImGui::RadioButton("PBR", isPbr);

		if (isLegacy && switchPbr)
		{
			material.mType = erm::MaterialType::PBR;
			material.mData = &erm::PBMaterial::DEFAULT;
		}
		else if (isPbr && switchLegacy)
		{
			material.mType = erm::MaterialType::LEGACY;
			material.mData = &erm::Material::DEFAULT;
		}

		switch (configs.mMaterial.mType)
		{
			default:
				ERM_ASSERT(false);
			case erm::MaterialType::LEGACY:
				ShowLegacyMaterialDebug(engine, configs);
				break;
			case erm::MaterialType::PBR:
				ShowPBMaterialDebug(engine, configs);
				break;
		}*/

		ImGui::Unindent();
	}
}

void ShowLegacyMaterialDebug(erm::BindingConfigs& /*configs*/)
{
	/*static erm::Materials& all = engine.GetResourcesManager().GetMaterials();

	ShowPathOptions<erm::Materials, erm::Material>(configs, all);

	erm::Material* material = static_cast<erm::Material*>(configs.mMaterial.mData);

	ImGui::SliderFloat3("Ambient", &material->mAmbient.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("Diffuse", &material->mDiffuse.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("Specular", &material->mSpecular.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Shininess", &material->mShininess, 0.0f, 1000.0f);*/
}

void ShowPBMaterialDebug(erm::BindingConfigs& /*configs*/)
{
	/*static erm::PBMaterials& all = engine.GetResourcesManager().GetPBMaterials();

	ShowPathOptions<erm::PBMaterials, erm::PBMaterial>(configs, all);

	erm::PBMaterial* material = static_cast<erm::PBMaterial*>(configs.mMaterial.mData);

	ImGui::SliderFloat3("Albedo", &material->mAlbedo.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Metallic", &material->mMetallic, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &material->mRoughness, 0.0f, 1.0f);
	ImGui::SliderFloat("AO", &material->mAO, 0.0f, 10.0f);*/
}

} // namespace ImGui
