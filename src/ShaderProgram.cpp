#include "ShaderProgram.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

namespace erm {
	
	ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
	{
		std::string vertexSource = ParseShader(vertexPath);
		std::string fragmentSource = ParseShader(fragmentPath);
		mId = CreateShaderProgram(vertexSource, fragmentSource);
	}
	
	std::string ShaderProgram::ParseShader(const std::string& path) const
	{
		std::ifstream stream (path);
		
		std::string result;
		std::string line;
		
		while (getline(stream, line))
		{
			result += line + "\n";
		}
		
		return result;
	}
	
	unsigned int ShaderProgram::CompileShader(unsigned int type, const std::string& source) const
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
	
	unsigned int ShaderProgram::CreateShaderProgram(const std::string& vertex, const std::string& fragment) const
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
