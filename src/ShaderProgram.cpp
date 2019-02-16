#include "ShaderProgram.h"

#include <fstream>

namespace erm {
	
	ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
		: vertexSource(ParseShader(vertexPath))
		, fragmentSource(ParseShader(fragmentPath))
	{}
	
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
	
}
