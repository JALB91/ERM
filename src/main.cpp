#include "Utils.h"
#include "GameConfig.h"
#include "Game.h"

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
	
	const unsigned int kWidth = 640;
	const unsigned int kHeight = 480;
	
	erm::GameConfig kGameConfig { kWidth, kHeight };
	erm::Game kGame { kGameConfig };
	
}

static void OnSizeChanged(GLFWwindow* window, int width, int height);
static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
static void OnMousePos(GLFWwindow* window, double xPos, double yPos);

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
	
#if defined(__APPLE__)
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	const char* glsl_version = "#version 150";
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
	
	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glCullFace(GL_FRONT));
	GLCALL(glFrontFace(GL_CCW));
	
//	GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	
	GLCALL(glClearColor(0.25f, 0.25f, 0.25f, 1.0f));
	
	glfwSetWindowSizeCallback(window, OnSizeChanged);
	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMousePos);
	
	{
		kGame.Init();
		
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		
		while (!glfwWindowShouldClose(window))
		{
			kGame.OnUpdate(0.0f);
			
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			ImGui::Begin("Hello, world!");
			kGame.OnImGuiRender();
			ImGui::End();
			
			kGame.OnRender();
			
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
	kGame.OnSizeChanged(width, height);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (GLFW_PRESS == action)
		{
			kGame.OnMouseButtonPressed();
		}
		else if (GLFW_RELEASE == action)
		{
			kGame.OnMouseButtonReleased();
		}
	}
}

void OnMousePos(GLFWwindow* window, double xPos, double yPos)
{
	kGame.OnMousePos(xPos, yPos);
}
