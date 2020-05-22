#include "erm/debug/ImGuiHandle.h"

#include "erm/debug/ImGuiWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/window/Window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace {
	
	const char* const kGlslVersion = "#version 330";
	
}

namespace erm {
	
	ImGuiHandle::ImGuiHandle(Engine& engine)
		: mEngine(engine)
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(mEngine.GetWindow().GetWindow(), true);
		ImGui_ImplOpenGL3_Init(kGlslVersion);
	}
	
	ImGuiHandle::~ImGuiHandle()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void ImGuiHandle::OnRender()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ImGui::ShowEngineDebug(mEngine);
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	
}
