#pragma once

#include <string>

namespace erm {
	
	struct ShaderProgram
	{
		ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
		
		std::string vertexSource;
		std::string fragmentSource;
		
	private:
		std::string ParseShader(const std::string& path) const;
		
	};

}
