#pragma once

#include <string>

namespace erm {
	
	struct ShaderProgram
	{
		ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
		
		unsigned int mId;
		
	private:
		std::string ParseShader(const std::string& path) const;
		unsigned int CompileShader(unsigned int type, const std::string& source) const;
		unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) const;
		
	};

}
