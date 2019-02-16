#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>

#include <string>
#include <iostream>

namespace {

	unsigned int buffer;
	
	float positions[6] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.0f, 0.5f
	};
	
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
	
	unsigned int CreateShader(const std::string& vertex, const std::string& fragment)
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
	GLFWwindow* window;
	
	if (!glfwInit())
	{
		return -1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	if (!(window = glfwCreateWindow(640, 480, "ERM", nullptr, nullptr)))
	{
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, positions, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	
	return 0;
}
