#pragma once

#include "game/Game.h"

#include "window/Window.h"

#include "rendering/RenderContext.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowRenderingDebugWindow(erm::Game& game)
	{
		const erm::Window& window = game.GetWindow();
		erm::RenderContext& renderContext = game.GetRenderContext();
		
		if (ImGui::Begin("Game Debug", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
		{
			bool isDepthEnabled = renderContext.IsDepthEnabled();
			int depthFunction = renderContext.GetDepthFunction();
			ImGui::Checkbox("Depth test enabled", &isDepthEnabled);
			ImGui::InputInt("Depth function", &depthFunction);
			
			ImGui::Separator();
			
			bool isBlendEnabled = renderContext.IsBlendEnabled();
			int blendSrc = renderContext.GetBlendSourceFactor();
			int blendDst = renderContext.GetBlendDestinationFactor();
			ImGui::Checkbox("Blend enabled", &isBlendEnabled);
			ImGui::InputInt("Blend source", &blendSrc);
			ImGui::InputInt("Blend destination", &blendDst);
			
			ImGui::Separator();
			
			bool isCullFaceEnabled = renderContext.IsCullFaceEnabled();
			int cullFace = renderContext.GetCullFace();
			int cullFrontFace = renderContext.GetCullFrontFace();
			ImGui::Checkbox("Cull face enabled", &isCullFaceEnabled);
			ImGui::InputInt("Cull face", &cullFace);
			ImGui::InputInt("Front face", &cullFrontFace);
			
			ImGui::Separator();
			
			int polygonMode = renderContext.GetPolygonMode();
			ImGui::InputInt("Polygon mode", &polygonMode);
			
			ImGui::Separator();
			
			erm::math::vec4 clearColor = renderContext.GetClearColor();
			ImGui::ColorEdit4("Clear color", &clearColor.x);
			
			renderContext.SetDepthEnabled(isDepthEnabled);
			renderContext.SetDepthFunction(depthFunction);
			
			renderContext.SetBlendEnabled(isBlendEnabled);
			renderContext.SetBlendFunction(blendSrc, blendDst);
			
			renderContext.SetCullFaceEnabled(isCullFaceEnabled);
			renderContext.SetCullFace(cullFace);
			renderContext.SetFrontFace(cullFrontFace);
			
			renderContext.SetPolygonMode(polygonMode);
			
			renderContext.SetClearColor(clearColor);
		}
		
		const erm::math::vec2 winSize (window.GetWindowWidth(), window.GetWindowHeight());
		const erm::math::vec2 viewport = window.GetViewport();
		ImGui::SetWindowSize(ImVec2(viewport.x, winSize.y - viewport.y));
		ImGui::SetWindowPos(ImVec2((winSize.x - viewport.x) * 0.5f, winSize.y - (winSize.y - viewport.y)));
		ImGui::End();
	}
	
}
