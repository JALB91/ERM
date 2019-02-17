#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ShaderProgram.h"

#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

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
	
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	{
		float positions[12] = {
			-0.5f, -0.5f, 0.0f,	// BOTTOM-LEFT
			-0.5f, 0.5f, 0.0f,	// TOP-LEFT
			0.5f, -0.5f, 0.0f,	// BOTTOM-RIGHT
			0.5f, 0.5f, 0.0f	// TOP-RIGHT
		};

		unsigned int indices[6] = {
			0, 1, 2,
			1, 2, 3
		};
		
		unsigned int VAO;
		GLCALL(glGenVertexArrays(1, &VAO));
		GLCALL(glBindVertexArray(VAO));

		erm::VertexBuffer vb (positions, sizeof(positions));
		erm::IndexBuffer ib (indices, 6);

		GLCALL(glEnableVertexAttribArray(0));
		GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));

		erm::ShaderProgram shaderProgram ("res/shaders/basic.vert", "res/shaders/basic.frag");
		GLCALL(glUseProgram(shaderProgram.mId));
		
		GLCALL(int location = glGetUniformLocation(shaderProgram.mId, "u_Color"));
		ASSERT(location != -1);
		GLCALL(glUniform4f(location, 1.0f, 1.0f, 0.0f, 1.0f));
		
		while (!glfwWindowShouldClose(window))
		{
			GLCALL(glClear(GL_COLOR_BUFFER_BIT));
			
			GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
			
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		
		GLCALL(glDeleteBuffers(1, &VAO));
		GLCALL(glDeleteProgram(shaderProgram.mId));
	}
	
	glfwTerminate();
	
	return 0;
}
