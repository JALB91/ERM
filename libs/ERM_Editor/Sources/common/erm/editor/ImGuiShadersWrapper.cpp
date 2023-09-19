#include "erm/debug/ImGuiShadersWrapper.h"

#include "erm/engine/Engine.h"

#include <erm/resources/ResourcesManager.h>
#include <erm/resources/shaders/ShaderUtils.h>

#include <erm/rendering/shaders/IShaderProgram.h>
#include <erm/rendering/window/Window.h>

#include <erm/utils/Utils.h>

#include <imgui.h>

#include <unordered_map>

namespace ImGui {

void ShowShadersDebug(erm::Engine& engine, bool& open)
{
	const std::vector<std::string>& shaders = engine.GetFileLocator().GetShaderPrograms();
	static erm::IShaderProgram* selected = nullptr;
	const erm::Window& window = engine.GetWindow();

	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.GetWindowWidth() / 2), static_cast<float>(window.GetWindowHeight() / 2)), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Shaders", &open, ImGuiWindowFlags_NoCollapse))
	{
		bool hasChanged = false;

		if (ImGui::BeginCombo("ShaderProgram", selected ? selected->mPath.c_str() : ""))
		{
			bool isSelected = selected;
			ImGui::PushID("NO_SELECTABLE");
			if (ImGui::Selectable("", &isSelected))
			{
				hasChanged = selected;
				selected = nullptr;
			}
			ImGui::PopID();

			for (unsigned int i = 0; i < shaders.size(); ++i)
			{
				isSelected = selected ? selected->mPath == shaders[i] : false;
				if (ImGui::Selectable(shaders[i].c_str(), &isSelected))
				{
					hasChanged = true;
//					TODO: Damiano
//					selected = engine.GetResourcesManager().GetOrCreateShaderProgram(shaders[i].c_str());
				}
			}
			ImGui::EndCombo();
		}

		static erm::ShaderSources shaderSources;
		static char current[1024 * 16] = "";

		if (selected && hasChanged)
		{
			shaderSources.clear();
			const erm::ShadersDataMap& dataMap = selected->GetShadersDataMap();

			for (const auto& [type, data] : dataMap)
			{
				shaderSources.emplace(
					std::piecewise_construct,
					std::forward_as_tuple(type),
					std::forward_as_tuple(data.size()));
				auto& sources = shaderSources[type];

				for (u64 i = 0; i < data.size(); ++i)
				{
					sources[i] = data[i].mShaderSource;
				}
			}
		}
		else if (hasChanged)
		{
			shaderSources.clear();
		}

		if (selected && ImGui::BeginTabBar("Shaders"))
		{
			const auto& dataMap = selected->GetShadersDataMap();
			for (const auto& [type, data] : dataMap)
			{
				for (u64 i = 0; i < data.size(); ++i)
				{
					std::string shaderId = erm::ShaderUtils::GetSuffixForShaderIndex(static_cast<u32>(i)).append(erm::ShaderUtils::GetExtensionForShaderType(type));

					if (ImGui::BeginTabItem(shaderId.c_str()))
					{
						strcpy(current, shaderSources[type][i].c_str());
						ImGui::InputTextMultiline(shaderId.c_str(), current, IM_ARRAYSIZE(current), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 50), ImGuiInputTextFlags_AllowTabInput);
						ImGui::EndTabItem();

						shaderSources[type][i] = current;
					}
				}
			}

			ImGui::EndTabBar();
		}

		if (selected)
		{
			ImGui::Separator();

			if (ImGui::Button("Save"))
			{
				selected->SetShadersSources(shaderSources);
			}
		}
	}

	ImGui::End();
}

} // namespace ImGui
