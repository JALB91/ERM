#include "rendering/ShaderProgram.h"
#include "rendering/Uniforms.h"

#include "utils/Utils.h"

#include <GL/glew.h>

#include <iostream>
#include <fstream>

namespace {
	
	const std::unordered_map<erm::Uniform, const char* const> kUniforms
	{
		{ erm::Uniform::MVP, "u_MVP" },
		{ erm::Uniform::NORMAL, "u_Normal" },
		{ erm::Uniform::TEXTURE_2D, "u_Texture" },
		{ erm::Uniform::MODEL, "u_Model" },
		{ erm::Uniform::VIEW, "u_View" },
		{ erm::Uniform::PROJECTION, "u_Projection" },
		{ erm::Uniform::MATERIAL_AMBIENT, "u_Material.ambient" },
		{ erm::Uniform::MATERIAL_DIFFUSE, "u_Material.diffuse" },
		{ erm::Uniform::MATERIAL_SPECULAR, "u_Material.specular" },
		{ erm::Uniform::MATERIAL_SHININESS, "u_Material.shininess" },
		{ erm::Uniform::LIGHT_POSITION, "u_Light.position" },
		{ erm::Uniform::LIGHT_AMBIENT, "u_Light.ambient" },
		{ erm::Uniform::LIGHT_DIFFUSE, "u_Light,diffuse" },
		{ erm::Uniform::LIGHT_SPECULAR, "u_Light.specular" },
		{ erm::Uniform::VIEW_POS, "u_ViewPos" }
	};
	
}

namespace erm {
	
	bool ShaderProgram::Create(
		const std::string& path,
		std::deque<ShaderProgram>& shaderProgramsContainer
	)
	{
		std::ifstream stream (Utils::GetRelativePath((path + ".vert").c_str()));
		if (!stream.is_open())
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}
		stream = std::ifstream(Utils::GetRelativePath((path + ".frag").c_str()));
		if (!stream.is_open())
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}
		
		shaderProgramsContainer.emplace_back(path);
		
		return true;
	}
	
	ShaderProgram::ShaderProgram(const std::string& shaderPath)
		: ShaderProgram(
			(shaderPath + ".vert").c_str(),
			(shaderPath + ".frag").c_str()
		)
	{
		mPath = shaderPath;
	}
	
	ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath)
		: mRendererId(
			CreateShaderProgram(
				ParseShader(Utils::GetRelativePath(vertexPath)),
				ParseShader(Utils::GetRelativePath(fragmentPath))
			)
		)
	{
		CacheUniformsLocations();
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
	
	void ShaderProgram::SetUniform1i(const Uniform& uniform, int value) const
	{
		GLCALL(glUniform1i(GetUniformLocation(kUniforms.at(uniform)), value));
	}
	
	void ShaderProgram::SetUniform1f(const Uniform& uniform, float value) const
	{
		GLCALL(glUniform1f(GetUniformLocation(kUniforms.at(uniform)), value));
	}
	
	void ShaderProgram::SetUniform3f(const Uniform& uniform, float v0, float v1, float v2) const
	{
		GLCALL(glUniform3f(GetUniformLocation(kUniforms.at(uniform)), v0, v1, v2));
	}
	
	void ShaderProgram::SetUniform4f(const Uniform& uniform, float v0, float v1, float v2, float v3) const
	{
		GLCALL(glUniform4f(GetUniformLocation(kUniforms.at(uniform)), v0, v1, v2, v3));
	}
	
	void ShaderProgram::SetUniformMat4f(const Uniform& uniform, const math::mat4& matrix) const
	{
		GLCALL(glUniformMatrix4fv(GetUniformLocation(kUniforms.at(uniform)), 1, GL_FALSE, &matrix[0][0]));
	}
	
	std::string ShaderProgram::ParseShader(const std::string& path) const
	{
		std::ifstream stream (path);
		
		ASSERT(stream.is_open());
		
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
			char* infoLog = (char*)alloca(lenght * sizeof(char));
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
	
	void ShaderProgram::CacheUniformsLocations()
	{
		for (const auto& uniform: kUniforms)
		{
			GLCALL(mUniformLocationsCache[uniform.second] = glGetUniformLocation(mRendererId, uniform.second));
		}
	}
	
	int ShaderProgram::GetUniformLocation(const std::string& name) const
	{
		if (mUniformLocationsCache.find(name) != mUniformLocationsCache.end())
		{
			return mUniformLocationsCache.at(name);
		}
		
		std::cout << "Uniform location not found" << std::endl;
		
		return -1;
	}
	
}
