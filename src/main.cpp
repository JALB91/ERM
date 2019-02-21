#include "Renderer.h"
#include "Scene.h"

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
	const float kSize = 50.0f;
	
	const float kPositions[24] = {
		// FRONT
		-kSize, -kSize, -kSize,	// 0 -- BOTTOM-LEFT
		-kSize,  kSize, -kSize,	// 1 -- TOP-LEFT
		 kSize, -kSize, -kSize,	// 2 -- BOTTOM-RIGHT
		 kSize,  kSize, -kSize,	// 3 -- TOP-RIGHT
		// BACK
		-kSize, -kSize,  kSize,	// 4 -- BOTTOM-LEFT
		-kSize,  kSize,  kSize,	// 5 -- TOP-LEFT
		 kSize, -kSize,  kSize,	// 6 -- BOTTOM-RIGHT
		 kSize,  kSize,  kSize,	// 7 -- TOP-RIGHT
	};
	
	const unsigned int kIndices[36] = {
		// FRONT FACE
		0, 1, 2,
		1, 2, 3,
		// BACK FACE
		4, 5, 6,
		5, 6, 7,
		// LEFT FACE
		0, 1, 4,
		4, 5, 0,
		// RIGHT FACE
		2, 3, 6,
		6, 7, 3,
		// TOP FACE
		1, 3, 5,
		5, 7, 3,
		// BOTTOM FACE
		0, 2, 4,
		4, 6, 0
	};
	
}

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
	
	GLCALL(glEnable(GL_BLEND));
	GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	
	{
		erm::Renderer renderer (kWidth, kHeight);
		erm::Scene scene (kPositions, 24, kIndices, 36);
		
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		
		while (!glfwWindowShouldClose(window))
		{
			scene.OnUpdate();
			
			renderer.Clear();
			
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			ImGui::Begin("Hello, world!");
			scene.OnImGuiRender();
			ImGui::End();
			
			scene.OnRender(renderer);
			
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
