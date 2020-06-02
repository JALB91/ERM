#include "erm/debug/ImGuiRenderingWrapper.h"
#include "erm/debug/ImGuiUtils.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/enums/PolygonMode.h"
#include "erm/rendering/window/Window.h"

#include <imgui.h>

#include <tuple>
#include <vector>

namespace {
	const std::vector<std::pair<const char*, erm::DepthFunction>> kDepthFnValues {
		{"NEVER", erm::DepthFunction::NEVER},
		{"LESS", erm::DepthFunction::LESS},
		{"EQUAL", erm::DepthFunction::EQUAL},
		{"LEQUAL", erm::DepthFunction::LEQUAL},
		{"GREATER", erm::DepthFunction::GREATER},
		{"NOT_EQUAL", erm::DepthFunction::NOT_EQUAL},
		{"GEQUAL", erm::DepthFunction::GEQUAL},
		{"ALWAYS", erm::DepthFunction::ALWAYS}};

	const std::vector<std::pair<const char*, erm::BlendFunction>> kBlendFnValues {
		{"ZERO", erm::BlendFunction::ZERO},
		{"ONE", erm::BlendFunction::ONE},
		{"SRC_COLOR", erm::BlendFunction::SRC_COLOR},
		{"ONE_MINUS_SRC_COLOR", erm::BlendFunction::ONE_MINUS_SRC_COLOR},
		{"SRC_ALPHA", erm::BlendFunction::SRC_ALPHA},
		{"ONE_MINUS_SRC_ALPHA", erm::BlendFunction::ONE_MINUS_SRC_ALPHA},
		{"DST_ALPHA", erm::BlendFunction::DST_ALPHA},
		{"ONE_MINUS_DST_ALPHA", erm::BlendFunction::ONE_MINUS_DST_ALPHA},
		{"DST_COLOR", erm::BlendFunction::DST_COLOR},
		{"ONE_MINUS_DST_COLOR", erm::BlendFunction::ONE_MINUS_DST_COLOR},
		{"CONSTANT_COLOR", erm::BlendFunction::CONSTANT_COLOR},
		{"ONE_MINUS_CONSTANT_COLOR", erm::BlendFunction::ONE_MINUS_CONSTANT_COLOR},
		{"CONSTANT_ALPHA", erm::BlendFunction::CONSTANT_ALPHA},
		{"ONE_MINUS_CONSTANT_ALPHA", erm::BlendFunction::ONE_MINUS_CONSTANT_ALPHA}};

	const std::vector<std::pair<const char*, erm::CullMode>> kCullModes {
		{"FRONT", erm::CullMode::FRONT},
		{"BACK", erm::CullMode::BACK},
		{"FRONT_AND_BACK", erm::CullMode::FRONT_AND_BACK}};

	const std::vector<std::pair<const char*, erm::FrontFace>> kFrontFaces {
		{"CW", erm::FrontFace::CW},
		{"CCW", erm::FrontFace::CCW}};

	const std::vector<std::pair<const char*, erm::PolygonMode>> kPolygonModes {
		{"POINT", erm::PolygonMode::POINT},
		{"LINE", erm::PolygonMode::LINE},
		{"FILL", erm::PolygonMode::FILL}};
} // namespace

namespace ImGui {

	void ShowRenderingDebugWindow(erm::Engine& engine)
	{
		const erm::Window& window = engine.GetWindow();
		//		erm::RenderContext& renderContext = engine.GetRenderContext();

		const int flags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs;

		if (ImGui::Begin("Engine Debug", nullptr, flags))
		{
			//			bool isDepthEnabled = renderContext.IsDepthEnabled();
			//			erm::DepthFunction depthFunction = renderContext.GetDepthFunction();
			//			ImGui::Checkbox("Depth test enabled", &isDepthEnabled);
			//			ImGui::ShowComboOf(kDepthFnValues, "Depth Function", depthFunction);
			//
			//			ImGui::Separator();
			//
			//			bool isBlendEnabled = renderContext.IsBlendEnabled();
			//			erm::BlendFunction blendSrc = renderContext.GetBlendSourceFactor();
			//			erm::BlendFunction blendDst = renderContext.GetBlendDestinationFactor();
			//			ImGui::Checkbox("Blend enabled", &isBlendEnabled);
			//			ImGui::ShowComboOf(kBlendFnValues, "Blend Source", blendSrc);
			//			ImGui::ShowComboOf(kBlendFnValues, "Blend Destination", blendDst);
			//
			//			ImGui::Separator();
			//
			//			bool isCullFaceEnabled = renderContext.IsCullFaceEnabled();
			//			erm::CullMode cullMode = renderContext.GetCullMode();
			//			erm::FrontFace frontFace = renderContext.GetFrontFace();
			//			ImGui::Checkbox("Cull face enabled", &isCullFaceEnabled);
			//			ImGui::ShowComboOf(kCullModes, "Cull Mode", cullMode);
			//			ImGui::ShowComboOf(kFrontFaces, "Front Face", frontFace);
			//
			//			ImGui::Separator();
			//
			//			erm::PolygonMode polygonMode = renderContext.GetPolygonMode();
			//			ImGui::ShowComboOf(kPolygonModes, "Polygon Mode", polygonMode);
			//
			//			ImGui::Separator();
			//
			//			erm::math::vec4 clearColor = renderContext.GetClearColor();
			//			ImGui::ColorEdit4("Clear color", &clearColor.x);
			//
			//			renderContext.SetDepthEnabled(isDepthEnabled);
			//			renderContext.SetDepthFunction(depthFunction);
			//
			//			renderContext.SetBlendEnabled(isBlendEnabled);
			//			renderContext.SetBlendFunction(blendSrc, blendDst);
			//
			//			renderContext.SetCullFaceEnabled(isCullFaceEnabled);
			//			renderContext.SetCullMode(cullMode);
			//			renderContext.SetFrontFace(frontFace);
			//
			//			renderContext.SetPolygonMode(polygonMode);
			//
			//			renderContext.SetClearColor(clearColor);
		}

		const erm::math::vec2 winSize(window.GetWindowWidth(), window.GetWindowHeight());
		const erm::BoundingBox2D& viewport = window.GetViewport();
		const erm::math::vec2 viewportSize = viewport.GetSize();
		ImGui::SetWindowSize(ImVec2(winSize.x, winSize.y - viewportSize.y));
		ImGui::SetWindowPos(ImVec2(0.0f, viewport.mMax.y));
		ImGui::End();
	}

} // namespace ImGui
