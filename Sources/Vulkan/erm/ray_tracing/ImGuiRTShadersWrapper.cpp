#include "erm/ray_tracing/ImGuiRTShadersWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/ray_tracing/RTShaderProgram.h"

#include "erm/rendering/window/Window.h"

#include "erm/utils/Utils.h"

#include <imgui.h>

namespace ImGui {

	void ShowRTShadersDebug(erm::Engine& engine, bool& open)
	{
		const std::vector<std::string>& shaders = engine.GetFileLocator().GetRTShaderPrograms();
		static erm::RTShaderProgram* selected = nullptr;
		const erm::Window& window = engine.GetWindow();

		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.GetWindowWidth() / 2), static_cast<float>(window.GetWindowHeight() / 2)), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("RTShaders", &open, ImGuiWindowFlags_NoCollapse))
		{
			bool hasChanged = false;

			if (ImGui::BeginCombo("RTShaderProgram", selected ? selected->mPath.c_str() : ""))
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
						selected = engine.GetResourcesManager().GetOrCreateRTShaderProgram(shaders[i].c_str());
					}
				}
				ImGui::EndCombo();
			}

			static std::map<erm::ShaderType, std::vector<std::string>> shaderSources;
			static char current[1024 * 16] = "";

			if (selected && hasChanged)
			{
				const std::map<erm::ShaderType, std::vector<erm::ShaderData>>& dataMap = selected->GetShadersDataMap();

				for (const auto& [type, data] : dataMap)
				{
					shaderSources.emplace(
						std::piecewise_construct,
						std::forward_as_tuple(type),
						std::forward_as_tuple(data.size()));
					auto& sources = shaderSources[type];

					for (size_t i = 0; i < data.size(); ++i)
					{
						sources[i] = data[i].mShaderSource;
					}
				}
			}
			else if (hasChanged)
			{
				shaderSources.clear();
			}

			if (selected && ImGui::BeginTabBar("RTShaders"))
			{
				const auto& dataMap = selected->GetShadersDataMap();
				for (const auto& [type, data] : dataMap)
				{
					for (size_t i = 0; i < data.size(); ++i)
					{
						std::string shaderId = erm::IShaderProgram::GetSuffixForShaderIndex(static_cast<uint32_t>(i)) + erm::IShaderProgram::GetExtensionForShaderType(type);

						if (ImGui::BeginTabItem(shaderId.c_str()))
						{
							std::strcpy(current, shaderSources[type][i].c_str());
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
					selected->SetShaderSources(shaderSources);
				}
			}
		}

		ImGui::End();
	}

} // namespace ImGui