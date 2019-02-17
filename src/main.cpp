#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "ShaderProgram.h"

#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

namespace {
	
	const float kPositions[12] = {
		-0.5f, -0.5f, 0.0f,	// BOTTOM-LEFT
		-0.5f, 0.5f, 0.0f,	// TOP-LEFT
		0.5f, -0.5f, 0.0f,	// BOTTOM-RIGHT
		0.5f, 0.5f, 0.0f	// TOP-RIGHT
	};
	
	const unsigned int kIndices[6] = {
		0, 1, 2,
		1, 2, 3
	};
	
	float r = 0.0f;
	float increment = 0.001f;
	
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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	
	window = glfwCreateWindow(640, 480, "ERM", nullptr, nullptr);
	
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
	
	{
		erm::VertexBuffer vb (kPositions, sizeof(kPositions));
		erm::VertexBufferLayout vbl;
		vbl.Push<float>(3);
		
		erm::VertexArray va;
		va.AddBuffer(vb, vbl);
		
		erm::IndexBuffer ib (kIndices, 6);

		erm::ShaderProgram shaderProgram ("res/shaders/basic.vert", "res/shaders/basic.frag");
		
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shaderProgram.Unbind();
		
		while (!glfwWindowShouldClose(window))
		{
			GLCALL(glClear(GL_COLOR_BUFFER_BIT));
			
			va.Bind();
			ib.Bind();
			shaderProgram.Bind();
			shaderProgram.SetUniform4f("u_Color", r, 0.2f, 0.5f, 1.0f);
			
			GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
			
			glfwSwapBuffers(window);
			glfwPollEvents();
			
			if (r > 1.0f || r < 0.0f)
			{
				increment = - increment;
			}
			
			r += increment;
		}
	}
	
	glfwTerminate();
	
	return 0;
}
