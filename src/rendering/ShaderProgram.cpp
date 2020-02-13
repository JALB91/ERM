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
		{ erm::Uniform::VIEW_PROJECTION, "u_ViewProjection" },
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
	
	unsigned int ShaderProgram::sShaderId = 0;
	
	ShaderProgram::ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
		: mRendererId(CreateShaderProgram(vertexShader, fragmentShader))
		, mPath("TEMP_" + std::to_string(sShaderId))
		, mVertex(vertexShader)
		, mFragment(fragmentShader)
		, mId(sShaderId++)
	{
		CacheUniformsLocations();
	}
	
	ShaderProgram::ShaderProgram(const std::string& shaderPath)
		: ShaderProgram(
			ParseShader(Utils::GetRelativePath((shaderPath + ".vert").c_str())),
			ParseShader(Utils::GetRelativePath((shaderPath + ".frag").c_str()))
		)
	{
		mPath = shaderPath;
	}
	
	ShaderProgram::~ShaderProgram()
	{
		GL_CALL(glDeleteProgram(mRendererId));
	}
	
	void ShaderProgram::Bind() const
	{
		GL_CALL(glUseProgram(mRendererId));
	}
	
	void ShaderProgram::Unbind() const
	{
		GL_CALL(glUseProgram(0));
	}
	
	void ShaderProgram::SetShaderSources(const std::string& vertexShader, const std::string& fragmentShader)
	{
		GL_CALL(glDeleteProgram(mRendererId));
		mRendererId = CreateShaderProgram(vertexShader, fragmentShader);
		mVertex = vertexShader;
		mFragment = fragmentShader;
		mUniformLocationsCache.clear();
		CacheUniformsLocations();
	}
	
	void ShaderProgram::SetUniform1i(const Uniform& uniform, int value) const
	{
		GL_CALL(glUniform1i(GetUniformLocation(kUniforms.at(uniform)), value));
	}
	
	void ShaderProgram::SetUniform1f(const Uniform& uniform, float value) const
	{
		GL_CALL(glUniform1f(GetUniformLocation(kUniforms.at(uniform)), value));
	}
	
	void ShaderProgram::SetUniform3f(const Uniform& uniform, float v0, float v1, float v2) const
	{
		GL_CALL(glUniform3f(GetUniformLocation(kUniforms.at(uniform)), v0, v1, v2));
	}
	
	void ShaderProgram::SetUniform4f(const Uniform& uniform, float v0, float v1, float v2, float v3) const
	{
		GL_CALL(glUniform4f(GetUniformLocation(kUniforms.at(uniform)), v0, v1, v2, v3));
	}
	
	void ShaderProgram::SetUniformMat4f(const Uniform& uniform, const math::mat4& matrix) const
	{
		GL_CALL(glUniformMatrix4fv(GetUniformLocation(kUniforms.at(uniform)), 1, GL_FALSE, &matrix[0][0]));
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
		GL_CALL(unsigned int id = glCreateShader(type));
		const char* src = source.c_str();
		
		GL_CALL(glShaderSource(id, 1, &src, nullptr));
		GL_CALL(glCompileShader(id));
		
		int result;
		GL_CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
		
		if (result == GL_FALSE)
		{
			int lenght;
			GL_CALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght));
			char* infoLog = (char*)alloca(lenght * sizeof(char));
			GL_CALL(glGetShaderInfoLog(id, lenght, &lenght, infoLog));
			
			std::cout << infoLog << std::endl;
			
			GL_CALL(glDeleteShader(id));
			
			return 0;
		}
		
		return id;
	}
	
	unsigned int ShaderProgram::CreateShaderProgram(const std::string& vertex, const std::string& fragment) const
	{
		GL_CALL(unsigned int program = glCreateProgram());
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment);
		
		GL_CALL(glAttachShader(program, vs));
		GL_CALL(glAttachShader(program, fs));
		GL_CALL(glLinkProgram(program));
		GL_CALL(glValidateProgram(program));
		
		GL_CALL(glDeleteShader(vs));
		GL_CALL(glDeleteShader(fs));
		
		return program;
	}
	
	void ShaderProgram::CacheUniformsLocations()
	{
		for (const auto& uniform: kUniforms)
		{
			GL_CALL(mUniformLocationsCache[uniform.second] = glGetUniformLocation(mRendererId, uniform.second));
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
