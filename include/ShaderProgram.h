#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace erm {
	
	class ShaderProgram
	{
	public:
		ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
		ShaderProgram(const std::string& shaderPath);
		~ShaderProgram();
		
		void Bind() const;
		void Unbind() const;
		
		void SetUniform1i(const std::string& name, int value);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
		
	private:
		std::string ParseShader(const std::string& path) const;
		unsigned int CompileShader(unsigned int type, const std::string& source) const;
		unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) const;
		
		int GetUniformLocation(const std::string& name);
		
		unsigned int mRendererId;
		std::unordered_map<std::string, int> mUniformLocationsCache;
		
	};

}
