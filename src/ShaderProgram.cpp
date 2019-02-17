#include "ShaderProgram.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

namespace erm {
	
	ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
	{
		mRendererId = CreateShaderProgram(ParseShader(vertexPath), ParseShader(fragmentPath));
	}
	
	ShaderProgram::~ShaderProgram()
	{
		GLCALL(glDeleteProgram(mRendererId));
	}
	
	void ShaderProgram::Bind() const
	{
		GLCALL(glUseProgram(mRendererId));
	}
	
	void ShaderProgram::Unbind() const
	{
		GLCALL(glUseProgram(0));
	}
	
	void ShaderProgram::SetUniform1i(const std::string &name, int value)
	{
		GLCALL(glUniform1i(GetUniformLocation(name), value));
	}
	
	void ShaderProgram::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		GLCALL(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
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
		GLCALL(unsigned int id = glCreateShader(type));
		const char* src = source.c_str();
		
		GLCALL(glShaderSource(id, 1, &src, nullptr));
		GLCALL(glCompileShader(id));
		
		int result;
		GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
		
		if (result == GL_FALSE)
		{
			int lenght;
			GLCALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght));
			char infoLog[lenght];
			GLCALL(glGetShaderInfoLog(id, lenght, &lenght, infoLog));
			
			std::cout << infoLog << std::endl;
			
			GLCALL(glDeleteShader(id));
			
			return 0;
		}
		
		return id;
	}
	
	unsigned int ShaderProgram::CreateShaderProgram(const std::string& vertex, const std::string& fragment) const
	{
		GLCALL(unsigned int program = glCreateProgram());
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment);
		
		GLCALL(glAttachShader(program, vs));
		GLCALL(glAttachShader(program, fs));
		GLCALL(glLinkProgram(program));
		GLCALL(glValidateProgram(program));
		
		GLCALL(glDeleteShader(vs));
		GLCALL(glDeleteShader(fs));
		
		return program;
	}
	
	int ShaderProgram::GetUniformLocation(const std::string& name)
	{
		if (mUniformLocationsCache.find(name) != mUniformLocationsCache.end())
		{
			return mUniformLocationsCache[name];
		}
		
		GLCALL(int location = glGetUniformLocation(mRendererId, name.c_str()));
		
		if (location == -1)
		{
			std::cout << "Uniform location not found" << std::endl;
		}
		
		mUniformLocationsCache[name] = location;
		
		return location;
	}
	
}
