#include "debug/ImGuiShadersWrapper.h"

#include "game/Game.h"

#include "window/Window.h"

#include "rendering/RenderContext.h"

#include "managers/ResourcesManager.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowShadersDebug(bool& open)
	{
		erm::Game& game = erm::Game::GetInstance();
		static erm::Shaders& shaders = game.GetResourcesManager().GetLoadedShaderPrograms();
		static erm::ShaderProgram* selected = nullptr;
		const erm::Window& window = game.GetWindow();
		
		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(window.GetWindowWidth() / 2, window.GetWindowHeight() / 2), ImGuiCond_FirstUseEver);
		
		if (ImGui::Begin("Shaders", &open, ImGuiWindowFlags_NoCollapse))
		{
			bool hasChanged = false;
			
			if (ImGui::BeginCombo("ShaderProgram", selected ? selected->GetPath().c_str() : ""))
			{
				bool isSelected = selected;
				if (ImGui::Selectable("", &isSelected))
				{
					hasChanged = selected;
					selected = nullptr;
				}
				
				for (unsigned int i = 0; i < shaders.size(); ++i)
				{
					bool isSelected = selected == shaders[i].get();
					if (ImGui::Selectable(shaders[i]->GetPath().c_str(), &isSelected))
					{
						hasChanged = selected != shaders[i].get();
						selected = shaders[i].get();
					}
				}
				ImGui::EndCombo();
			}
			
			static char vertexShader[1024 * 16] = "";
			static char fragmentShader[1024 * 16] = "";
			
			if (selected && hasChanged)
			{
				std::strcpy(vertexShader, selected->GetVertexShader().c_str());
				std::strcpy(fragmentShader, selected->GetFragmentShader().c_str());
			}
			else if (hasChanged)
			{
				std::strcpy(vertexShader, "");
				std::strcpy(fragmentShader, "");
			}
			
			ImGui::InputTextMultiline("Vertex", vertexShader, IM_ARRAYSIZE(vertexShader), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
			ImGui::Separator();
			ImGui::InputTextMultiline("Fragment", fragmentShader, IM_ARRAYSIZE(fragmentShader), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
			
			if (ImGui::Button("Save"))
			{
				if (selected)
				{
					selected->SetShaderSources(vertexShader, fragmentShader);
				}
				else
				{
					game.GetResourcesManager().GetOrCreateShaderProgram(vertexShader, fragmentShader);
				}
			}
		}
		
		ImGui::End();
	}
	
}