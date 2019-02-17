#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>

namespace {
	
	const float kWidth = 640.0f;
	const float kHeight = 480.0f;
	
	const float kPositions[16] = {
		kWidth * 0.25f, kHeight * 0.25f, 0.0f, 0.0f,	// BOTTOM-LEFT
		kWidth * 0.25f, kHeight * 0.75f, 0.0f, 1.0f,	// TOP-LEFT
		kWidth * 0.75f, kHeight * 0.25f, 1.0f, 0.0f,	// BOTTOM-RIGHT
		kWidth * 0.75f, kHeight * 0.75f, 1.0f, 1.0f		// TOP-RIGHT
	};
	
	const unsigned int kIndices[6] = {
		0, 1, 2,
		1, 2, 3
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
#ifdef __APPLE__
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
	
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	GLCALL(glEnable(GL_BLEND));
	GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	
	{
		glm::mat4 p = glm::ortho(0.0f, kWidth, 0.0f, kHeight, -1.0f, 1.0f);
		glm::mat4 v = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, 0.0f));
		glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		
		glm::mat4 mvp = p * v * m;
		
		erm::VertexBuffer vb (kPositions, sizeof(kPositions));
		erm::VertexBufferLayout vbl;
		vbl.Push<float>(2);
		vbl.Push<float>(2);
		
		erm::VertexArray va;
		va.AddBuffer(vb, vbl);
		
		erm::IndexBuffer ib (kIndices, 6);

		erm::ShaderProgram shader ("res/shaders/basic.vert", "res/shaders/basic.frag");
		shader.Bind();
		
		erm::Texture texture ("res/textures/smile.png");
		texture.Bind();
		
		shader.SetUniform1i("u_Texture", 0);
		shader.SetUniformMat4f("u_MVP", mvp);
		
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
		
		erm::Renderer renderer;
		
		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();
			
			renderer.Draw(va, ib, shader);
			
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	
	glfwTerminate();
	
	return 0;
}
