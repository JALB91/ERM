#include "erm/editor/ImGuiShadersWrapper.h"

//#include <erm/assets/ResourcesManager.h>
#include <erm/assets/shaders/ShaderUtils.h>
#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/rendering/shaders/IShaderProgram.h>
#include <erm/window/Window.h>
#include <erm/utils/Utils.h>

#include <imgui.h>

#include <unordered_map>

namespace ImGui {

void ShowShadersDebug(bool& open)
{
//	const std::vector<std::string>& shaders = engine.getFileLocator().getShaderPrograms();
	static erm::IShaderProgram* selected = nullptr;
	const auto window = erm::ObjectRegistry::get<erm::Window>();

	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window->getWindowWidth() / 2), static_cast<float>(window->getWindowHeight() / 2)), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Shaders", &open, ImGuiWindowFlags_NoCollapse))
	{
//		bool hasChanged = false;
//
//		if (ImGui::BeginCombo("ShaderProgram", selected ? selected->mPath.c_str() : ""))
//		{
//			bool isSelected = selected;
//			ImGui::PushID("NO_SELECTABLE");
//			if (ImGui::Selectable("", &isSelected))
//			{
//				hasChanged = selected;
//				selected = nullptr;
//			}
//			ImGui::PopID();
//
//			for (unsigned int i = 0; i < shaders.size(); ++i)
//			{
//				isSelected = selected ? selected->mPath == shaders[i] : false;
//				if (ImGui::Selectable(shaders[i].c_str(), &isSelected))
//				{
//					hasChanged = true;
////					TODO: Damiano
////					selected = engine.GetResourcesManager().GetOrCreateShaderProgram(shaders[i].c_str());
//				}
//			}
//			ImGui::EndCombo();
//		}

		//static erm::ShaderSources shaderSources;
		//static char current[1024 * 16] = "";

		//if (selected && hasChanged)
		//{
		//	shaderSources.clear();
		//	/*const erm::ShadersDataMap& dataMap = selected->getShadersDataMap();

		//	for (const auto& [type, data] : dataMap)
		//	{
		//		shaderSources.emplace(
		//			std::piecewise_construct,
		//			std::forward_as_tuple(type),
		//			std::forward_as_tuple(data.size()));
		//		auto& sources = shaderSources[type];

		//		for (erm::u64 i = 0; i < data.size(); ++i)
		//		{
		//			sources[i] = data[i].mShaderSource;
		//		}
		//	}*/
		//}
		//else if (hasChanged)
		//{
		//	shaderSources.clear();
		//}

		if (selected && ImGui::BeginTabBar("Shaders"))
		{
			/*const auto& dataMap = selected->getShadersDataMap();
			for (const auto& [type, data] : dataMap)
			{
				for (erm::u64 i = 0; i < data.size(); ++i)
				{
					std::string shaderId = erm::ShaderUtils::getSuffixForShaderIndex(static_cast<erm::u32>(i)).append(erm::ShaderUtils::getExtensionForShaderType(type));

					if (ImGui::BeginTabItem(shaderId.c_str()))
					{
						strcpy(current, shaderSources[type][i].c_str());
						ImGui::InputTextMultiline(shaderId.c_str(), current, IM_ARRAYSIZE(current), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 50), ImGuiInputTextFlags_AllowTabInput);
						ImGui::EndTabItem();

						shaderSources[type][i] = current;
					}
				}
			}*/

			ImGui::EndTabBar();
		}

		if (selected)
		{
			ImGui::Separator();

			if (ImGui::Button("Save"))
			{
				//selected->setShadersSources(shaderSources);
			}
		}
	}

	ImGui::End();
}

} // namespace ImGui
