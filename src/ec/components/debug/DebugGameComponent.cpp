#include "ec/components/debug/DebugGameComponent.h"
#include "ec/Entity.h"

#include "game/Game.h"

#include "rendering/Renderer.h"

#include "utils/Utils.h"

#include <imgui.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace erm {
	
	DebugGameComponent::DebugGameComponent(Entity& entity)
		: IComponent(entity)
		, mRenderer(mEntity.GetRenderer())
	{}
	
	DebugGameComponent::~DebugGameComponent()
	{}
	
	void DebugGameComponent::OnImGuiRender()
	{
		ImGui::Begin("Game Debug");
		
		bool isDepthEnabled = mRenderer.IsDepthEnabled();
		int depthFunction = mRenderer.GetDepthFunction();
		ImGui::Checkbox("Depth test enabled", &isDepthEnabled);
		ImGui::SameLine();
		ImGui::InputInt("Depth function", &depthFunction);
		
		bool isBlendEnabled = mRenderer.IsBlendEnabled();
		int blendSrc = mRenderer.GetBlendSourceFactor();
		int blendDst = mRenderer.GetBlendDestinationFactor();
		ImGui::Checkbox("Blend enabled", &isBlendEnabled);
		ImGui::SameLine();
		ImGui::InputInt("Blend source", &blendSrc);
		ImGui::SameLine();
		ImGui::InputInt("Blend destination", &blendDst);
		
		bool isCullFaceEnabled = mRenderer.IsCullFaceEnabled();
		int cullFace = mRenderer.GetCullFace();
		int cullFrontFace = mRenderer.GetCullFrontFace();
		ImGui::Checkbox("Cull face enabled", &isCullFaceEnabled);
		ImGui::SameLine();
		ImGui::InputInt("Cull face", &cullFace);
		ImGui::SameLine();
		ImGui::InputInt("Front face", &cullFrontFace);
		
		int polygonMode = mRenderer.GetPolygonMode();
		ImGui::InputInt("Polygon mode", &polygonMode);
		
		glm::vec4 clearColor = mRenderer.GetClearColor();
		ImGui::ColorPicker4("Clear color", &clearColor.x);
		
		mRenderer.SetDepthEnabled(isDepthEnabled);
		mRenderer.SetDepthFunction(depthFunction);
		
		mRenderer.SetBlendEnabled(isBlendEnabled);
		mRenderer.SetBlendFunction(blendSrc, blendDst);
		
		mRenderer.SetCullFaceEnabled(isCullFaceEnabled);
		mRenderer.SetCullFace(cullFace);
		mRenderer.SetFrontFace(cullFrontFace);
		
		mRenderer.SetPolygonMode(polygonMode);
		
		mRenderer.SetClearColor(clearColor);
		
		ImGui::End();
	}
	
}
