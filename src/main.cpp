#include "ShaderProgram.h"

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

namespace {

	unsigned int CompileShader(unsigned int type, const std::string& source)
	{
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);
		
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		
		if (result == GL_FALSE)
		{
			int lenght;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
			char infoLog[lenght];
			glGetShaderInfoLog(id, lenght, &lenght, infoLog);
			
			std::cout << infoLog << std::endl;
			
			glDeleteShader(id);
			
			return 0;
		}
		
		return id;
	}
	
	unsigned int CreateShaderProgram(const std::string& vertex, const std::string& fragment)
	{
		unsigned int program = glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment);
		
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);
		
		glDeleteShader(vs);
		glDeleteShader(fs);
		
		return program;
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
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
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
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	erm::ShaderProgram shaderProgram ("res/shaders/basic.vert", "res/shaders/basic.frag");
	unsigned int program = CreateShaderProgram(shaderProgram.vertexSource, shaderProgram.fragmentSource);

	glUseProgram(program);
	
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
//		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteBuffers(1, &buffer);
	glDeleteProgram(program);
	
	glfwTerminate();
	
	return 0;
}
