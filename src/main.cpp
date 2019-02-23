#include "Renderer.h"
#include "SceneObject.h"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <iostream>

namespace {
	
	const float kWidth = 640.0f;
	const float kHeight = 480.0f;
	
	erm::Renderer renderer (kWidth, kHeight);
	
}

void OnSizeChanged(GLFWwindow* window, int width, int height);

int main(int argc, char** argv)
{
	GLFWwindow* window = nullptr;
	
	if (!glfwInit())
	{
		std::cout << "GLFW initialization failed!" << std::endl;
		return -1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
#ifdef __APPLE__
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	window = glfwCreateWindow(kWidth, kHeight, "ERM", nullptr, nullptr);
	
	if (!window)
	{
		std::cout << "GLFW Window creation failed!" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization failed" << std::endl;
		return -1;
	}
	
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	GLCALL(glDepthFunc(GL_LESS));
	GLCALL(glEnable(GL_DEPTH_TEST));
	
	GLCALL(glEnable(GL_BLEND));
	GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	
//	GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	
	GLCALL(glClearColor(0.25f, 0.25f, 0.25f, 1.0f));
	
	glfwSetWindowSizeCallback(window, OnSizeChanged);
	
	{
		erm::SceneObject sceneObject;
		
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		
		while (!glfwWindowShouldClose(window))
		{
			sceneObject.OnUpdate();
			
			renderer.Clear();
			
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			ImGui::Begin("Hello, world!");
			sceneObject.OnImGuiRender();
			ImGui::End();
			
			sceneObject.OnRender(renderer);
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	
	return 0;
}

void OnSizeChanged(GLFWwindow* window, int width, int height)
{
	renderer.OnSizeChanged(width, height);
}
