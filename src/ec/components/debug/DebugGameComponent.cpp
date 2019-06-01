#include "ec/components/debug/DebugGameComponent.h"
#include "ec/Entity.h"

#include "game/Game.h"

#include "rendering/RenderContext.h"

#include "utils/Utils.h"

#include <imgui.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace erm {
	
	DebugGameComponent::DebugGameComponent(
		Entity& entity,
		const RenderContext& renderContext
		)
		: IComponent(entity)
		, mRenderContext(renderContext)
	{}
	
	DebugGameComponent::~DebugGameComponent()
	{}
	
	void DebugGameComponent::OnImGuiRender()
	{
		if (ImGui::Begin("Game Debug"))
		{
			bool isDepthEnabled = mRenderContext.IsDepthEnabled();
			int depthFunction = mRenderContext.GetDepthFunction();
			ImGui::Checkbox("Depth test enabled", &isDepthEnabled);
			ImGui::InputInt("Depth function", &depthFunction);
			
			ImGui::Separator();
			
			bool isBlendEnabled = mRenderContext.IsBlendEnabled();
			int blendSrc = mRenderContext.GetBlendSourceFactor();
			int blendDst = mRenderContext.GetBlendDestinationFactor();
			ImGui::Checkbox("Blend enabled", &isBlendEnabled);
			ImGui::InputInt("Blend source", &blendSrc);
			ImGui::InputInt("Blend destination", &blendDst);
			
			ImGui::Separator();
			
			bool isCullFaceEnabled = mRenderContext.IsCullFaceEnabled();
			int cullFace = mRenderContext.GetCullFace();
			int cullFrontFace = mRenderContext.GetCullFrontFace();
			ImGui::Checkbox("Cull face enabled", &isCullFaceEnabled);
			ImGui::InputInt("Cull face", &cullFace);
			ImGui::InputInt("Front face", &cullFrontFace);
			
			ImGui::Separator();
			
			int polygonMode = mRenderContext.GetPolygonMode();
			ImGui::InputInt("Polygon mode", &polygonMode);
			
			ImGui::Separator();
			
			glm::vec4 clearColor = mRenderContext.GetClearColor();
			ImGui::ColorEdit4("Clear color", &clearColor.x);
			
			mRenderContext.SetDepthEnabled(isDepthEnabled);
			mRenderContext.SetDepthFunction(depthFunction);
			
			mRenderContext.SetBlendEnabled(isBlendEnabled);
			mRenderContext.SetBlendFunction(blendSrc, blendDst);
			
			mRenderContext.SetCullFaceEnabled(isCullFaceEnabled);
			mRenderContext.SetCullFace(cullFace);
			mRenderContext.SetFrontFace(cullFrontFace);
			
			mRenderContext.SetPolygonMode(polygonMode);
			
			mRenderContext.SetClearColor(clearColor);
			
			ImGui::End();
		}
	}
	
}
