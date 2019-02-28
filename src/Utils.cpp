#include "Utils.h"

#include <GL/glew.h>

#include <iostream>

namespace erm {
	
	void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}
	
	bool GLLogCall(const char* function, const char* file, int line)
	{
		while (GLenum error = glGetError())
		{
			std::cout << "[OpenGL error] (" << error << ") " << function << " " << file << ":" << line << std::endl;
			return false;
		}
		return true;
	}
	
	std::vector<std::string> SplitString(const std::string& str, char ch)
	{
		std::vector<std::string> vec;
		std::string res = "";
		
		for (auto iter = str.cbegin(); iter != str.cend(); ++iter)
		{
			if (*iter != ch)
			{
				res += *iter;
			}
			else
			{
				vec.push_back(res);
				res = "";
			}
		}
		
		vec.push_back(res);
		
		return vec;
	}

	std::string GetRelativePath(const char* absolutePath)
	{
#if defined(WIN32)
		const std::string result = std::string("../") + absolutePath;
#elif defined(__APPLE__)
		const std::string result = absolutePath;
#endif
		return result;
	}
	
}
