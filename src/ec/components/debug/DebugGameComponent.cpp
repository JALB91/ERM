#include "ec/components/debug/DebugGameComponent.h"
#include "ec/Entity.h"

#include "game/Game.h"

#include "window/Window.h"

#include "utils/Utils.h"

#include <imgui.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace erm {
	
	DebugGameComponent::DebugGameComponent(Entity& entity)
		: IComponent(entity)
		, mWindow(mEntity.GetWindow())
	{}
	
	DebugGameComponent::~DebugGameComponent()
	{}
	
	void DebugGameComponent::OnImGuiRender()
	{
		ImGui::Begin("Game Debug");
		
		bool isDepthEnabled = mWindow.IsDepthEnabled();
		int depthFunction = mWindow.GetDepthFunction();
		ImGui::Checkbox("Depth test enabled", &isDepthEnabled);
		ImGui::SameLine();
		ImGui::InputInt("Depth function", &depthFunction);
		
		bool isBlendEnabled = mWindow.IsBlendEnabled();
		int blendSrc = mWindow.GetBlendSourceFactor();
		int blendDst = mWindow.GetBlendDestinationFactor();
		ImGui::Checkbox("Blend enabled", &isBlendEnabled);
		ImGui::SameLine();
		ImGui::InputInt("Blend source", &blendSrc);
		ImGui::SameLine();
		ImGui::InputInt("Blend destination", &blendDst);
		
		bool isCullFaceEnabled = mWindow.IsCullFaceEnabled();
		int cullFace = mWindow.GetCullFace();
		int cullFrontFace = mWindow.GetCullFrontFace();
		ImGui::Checkbox("Cull face enabled", &isCullFaceEnabled);
		ImGui::SameLine();
		ImGui::InputInt("Cull face", &cullFace);
		ImGui::SameLine();
		ImGui::InputInt("Front face", &cullFrontFace);
		
		int polygonMode = mWindow.GetPolygonMode();
		ImGui::InputInt("Polygon mode", &polygonMode);
		
		glm::vec4 clearColor = mWindow.GetClearColor();
		ImGui::ColorPicker4("Clear color", &clearColor.x);
		
		mWindow.SetDepthEnabled(isDepthEnabled);
		mWindow.SetDepthFunction(depthFunction);
		
		mWindow.SetBlendEnabled(isBlendEnabled);
		mWindow.SetBlendFunction(blendSrc, blendDst);
		
		mWindow.SetCullFaceEnabled(isCullFaceEnabled);
		mWindow.SetCullFace(cullFace);
		mWindow.SetFrontFace(cullFrontFace);
		
		mWindow.SetPolygonMode(polygonMode);
		
		mWindow.SetClearColor(clearColor);
		
		ImGui::End();
	}
	
}
