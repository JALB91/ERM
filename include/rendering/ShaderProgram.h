#pragma once

#include "math/mat.h"

#include <string>
#include <unordered_map>

namespace erm {
	
	enum class Uniform;
	
	class ShaderProgram
	{
	public:
		ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
		ShaderProgram(const std::string& shaderPath);
		~ShaderProgram();
		
		void Bind() const;
		void Unbind() const;
		
		void SetUniform1i(const Uniform& uniform, int value) const;
		void SetUniform1f(const Uniform& uniform, float value) const;
		void SetUniform3f(const Uniform& uniform, float v0, float v1, float v2) const;
		void SetUniform4f(const Uniform& uniform, float v0, float v1, float v2, float v3) const;
		void SetUniformMat4f(const Uniform& uniform, const math::mat4& matrix) const;
		
	private:
		std::string ParseShader(const std::string& path) const;
		unsigned int CompileShader(unsigned int type, const std::string& source) const;
		unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) const;
		
		void CacheUniformsLocations();
		int GetUniformLocation(const std::string& name) const;
		
		unsigned int mRendererId;
		std::unordered_map<std::string, int> mUniformLocationsCache;
		
	};

}
