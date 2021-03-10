#include "erm/debug/ImGuiMaterialWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/PBMaterial.h"

#include "erm/utils/Utils.h"

#include <imgui.h>

#include <set>

namespace ImGui {

	void ShowPBMaterialDebug(erm::Engine& engine, erm::Mesh& mesh)
	{
		if (ImGui::CollapsingHeader("PBMaterial"))
		{
			erm::PBMaterial* material = mesh.GetRenderConfigs().mPBMaterial ? mesh.GetRenderConfigs().mPBMaterial : &erm::PBMaterial::DEFAULT;

			ImGui::Indent();

			ImGui::SliderFloat3("Albedo", &material->mAlbedo.x, 0.0f, 1.0f);
			ImGui::SliderFloat("Metallic", &material->mMetallic, 0.0f, 1.0f);
			ImGui::SliderFloat("Roughness", &material->mRoughness, 0.0f, 1.0f);
			ImGui::SliderFloat("AO", &material->mAO, 0.0f, 10.0f);

			ImGui::Unindent();
		}
	}

} // namespace ImGui
