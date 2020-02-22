#include "debug/ImGuiRenderingWrapper.h"
#include "debug/ImGuiUtils.h"

#include "game/Game.h"

#include "window/Window.h"

#include "rendering/RenderContext.h"

#include <imgui.h>

#include <tuple>
#include <vector>

namespace {
	const std::vector<std::pair<const char*, int>> kDepthFnValues {
		{ "NEVER", 		0x0200 },
		{ "LESS", 		0x0201 },
		{ "EQUAL", 		0x0202 },
		{ "LEQUAL", 	0x0203 },
		{ "GREATER", 	0x0204 },
		{ "NOTEQUAL", 	0x0205 },
		{ "GEQUAL", 	0x0206 },
		{ "ALWAYS", 	0x0207 }
	};
	
	const std::vector<std::pair<const char*, int>> kBlendFnValues {
		{ "ZERO", 						0 },
		{ "ONE", 						1 },
		{ "SRC_COLOR", 					0x0300 },
		{ "ONE_MINUS_SRC_COLOR", 		0x0301 },
		{ "SRC_ALPHA", 					0x0302 },
		{ "ONE_MINUS_SRC_ALPHA", 		0x0303 },
		{ "DST_ALPHA", 					0x0304 },
		{ "ONE_MINUS_DST_ALPHA", 		0x0305 },
		{ "DST_COLOR", 					0x0306 },
		{ "ONE_MINUS_DST_COLOR", 		0x0307 },
		{ "SRC_ALPHA_SATURATE", 		0x0308 },
		{ "CONSTANT_COLOR", 			0x8001 },
		{ "ONE_MINUS_CONSTANT_COLOR", 	0x8002 },
		{ "CONSTANT_ALPHA", 			0x8003 },
		{ "ONE_MINUS_CONSTANT_ALPHA", 	0x8004 }
	};
	
	const std::vector<std::pair<const char*, int>> kCullFace {
		{ "FRONT", 			0x0404 },
		{ "BACK", 			0x0405 },
		{ "FRONT_AND_BACK",	0x0408 }
	};
	
	const std::vector<std::pair<const char*, int>> kFrontFace {
		{ "CW", 	0x0900 },
		{ "CCW", 	0x0901 }
	};
	
	const std::vector<std::pair<const char*, int>> kPolygonMode {
		{ "POINT", 	0x1B00 },
		{ "LINE", 	0x1B01 },
		{ "FILL",	0x1B02 }
	};
}

namespace ImGui {
	
	void ShowRenderingDebugWindow(erm::Game& game)
	{
		const erm::Window& window = game.GetWindow();
		erm::RenderContext& renderContext = game.GetRenderContext();
		
		const int flags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs;
		
		if (ImGui::Begin("Game Debug", nullptr, flags))
		{
			bool isDepthEnabled = renderContext.IsDepthEnabled();
			int depthFunction = renderContext.GetDepthFunction();
			ImGui::Checkbox("Depth test enabled", &isDepthEnabled);
			ImGui::ShowComboOf(kDepthFnValues, "Depth Function", depthFunction);
			
			ImGui::Separator();
			
			bool isBlendEnabled = renderContext.IsBlendEnabled();
			int blendSrc = renderContext.GetBlendSourceFactor();
			int blendDst = renderContext.GetBlendDestinationFactor();
			ImGui::Checkbox("Blend enabled", &isBlendEnabled);
			ImGui::ShowComboOf(kBlendFnValues, "Blend Source", blendSrc);
			ImGui::ShowComboOf(kBlendFnValues, "Blend Destination", blendDst);
			
			ImGui::Separator();
			
			bool isCullFaceEnabled = renderContext.IsCullFaceEnabled();
			int cullFace = renderContext.GetCullFace();
			int frontFace = renderContext.GetFrontFace();
			ImGui::Checkbox("Cull face enabled", &isCullFaceEnabled);
			ImGui::ShowComboOf(kCullFace, "Cull Face", cullFace);
			ImGui::ShowComboOf(kFrontFace, "Front Face", frontFace);
			
			ImGui::Separator();
			
			int polygonMode = renderContext.GetPolygonMode();
			ImGui::ShowComboOf(kPolygonMode, "Polygon Mode", polygonMode);
			
			ImGui::Separator();
			
			erm::math::vec4 clearColor = renderContext.GetClearColor();
			ImGui::ColorEdit4("Clear color", &clearColor.x);
			
			renderContext.SetDepthEnabled(isDepthEnabled);
			renderContext.SetDepthFunction(depthFunction);
			
			renderContext.SetBlendEnabled(isBlendEnabled);
			renderContext.SetBlendFunction(blendSrc, blendDst);
			
			renderContext.SetCullFaceEnabled(isCullFaceEnabled);
			renderContext.SetCullFace(cullFace);
			renderContext.SetFrontFace(frontFace);
			
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
