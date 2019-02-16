#include "ShaderProgram.h"

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

namespace {

#define ASSERT(x) assert(x)
#define GL_CALL(x) \
	GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
	
	void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}
	
	bool GLLogCall(const char* function, const char* file, int line)
	{
		while (GLenum error = glGetError())
		{
			std::cout << "[OpenGL error] (" << error << ") " << function << " " << file << ":" << line << std::endl;
			return false;
		}
		return true;
	}
	
}

int main(int argc, char** argv)
{
	GLFWwindow* window = nullptr;
	
	if (!glfwInit())
	{
		return -1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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
	GL_CALL(glGenVertexArrays(1, &VAO));
	GL_CALL(glBindVertexArray(VAO));

	unsigned int VBO;
	GL_CALL(glGenBuffers(1, &VBO));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));

	unsigned int EBO;
	GL_CALL(glGenBuffers(1, &EBO));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));

	erm::ShaderProgram shaderProgram ("res/shaders/basic.vert", "res/shaders/basic.frag");
	GL_CALL(glUseProgram(shaderProgram.mId));
	
	GL_CALL(int location = glGetUniformLocation(shaderProgram.mId, "u_Color"));
	ASSERT(location != -1);
	GL_CALL(glUniform4f(location, 1.0f, 1.0f, 0.0f, 1.0f));
	
	while (!glfwWindowShouldClose(window))
	{
		GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
		
		GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	GL_CALL(glDeleteBuffers(1, &VAO));
	GL_CALL(glDeleteBuffers(1, &VBO));
	GL_CALL(glDeleteBuffers(1, &EBO));
	GL_CALL(glDeleteProgram(shaderProgram.mId));
	
	glfwTerminate();
	
	return 0;
}
