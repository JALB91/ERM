#include "erm/ray_tracing/ImGuiRTShadersWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/Utils.h"

#include <imgui.h>

namespace ImGui {

	void ShowShadersDebug(erm::Engine& engine, bool& open)
	{
		const std::vector<std::string>& shaders = engine.GetFileLocator().GetShaderPrograms();
		static erm::ShaderProgram* selected = nullptr;
		const erm::Window& window = engine.GetWindow();

		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.GetWindowWidth() / 2), static_cast<float>(window.GetWindowHeight() / 2)), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Shaders", &open, ImGuiWindowFlags_NoCollapse))
		{
			bool hasChanged = false;

			if (ImGui::BeginCombo("ShaderProgram", selected ? selected->mPath.c_str() : ""))
			{
				bool isSelected = selected;
				if (ImGui::Selectable("", &isSelected))
				{
					hasChanged = selected;
					selected = nullptr;
				}

				for (unsigned int i = 0; i < shaders.size(); ++i)
				{
					isSelected = selected ? selected->mPath == shaders[i] : false;
					if (ImGui::Selectable(shaders[i].c_str(), &isSelected))
					{
						hasChanged = true;
						selected = engine.GetResourcesManager().GetOrCreateShaderProgram(shaders[i].c_str());
					}
				}
				ImGui::EndCombo();
			}

			static std::map<erm::ShaderType, std::string> shaderSources;
			static char current[1024 * 16] = "";

			if (selected && hasChanged)
			{
				const std::map<erm::ShaderType, erm::ShaderData>& dataMap = selected->GetShadersDataMap();

				for (const auto& [type, data] : dataMap)
				{
					shaderSources.emplace(
						std::piecewise_construct,
						std::forward_as_tuple(type),
						std::forward_as_tuple(data.mShaderSource));
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
					const char* extension = erm::IShaderProgram::GetExtensionForShaderType(type);

					if (ImGui::BeginTabItem(extension))
					{
						std::strcpy(current, shaderSources[type].c_str());
						ImGui::InputTextMultiline(extension, current, IM_ARRAYSIZE(current), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 50), ImGuiInputTextFlags_AllowTabInput);
						ImGui::EndTabItem();

						shaderSources[type] = current;
					}
				}

				ImGui::EndTabBar();
			}

			if (selected)
			{
				ImGui::Separator();

				if (ImGui::Button("Save"))
				{
					selected->SetShaderSources(shaderSources);
				}
			}
		}

		ImGui::End();
	}

} // namespace ImGui
