#include "utils/Utils.h"

#include <GL/glew.h>

#include <iostream>
#include <sstream>
#include <math.h>

namespace erm {
	
	namespace Utils {
		
		bool LogCall(bool cond, const char* msg, const char* function, const char* file, int line)
		{
			if (!cond)
			{
				std::cout << "[Assert] (" << msg << ") " << function << " " << file << ":" << line << std::endl;
				ASSERT(cond);
			}
			return cond;
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
		
		void GLClearError()
		{
			while (glGetError() != GL_NO_ERROR);
		}

		std::deque<std::string> Utils::SplitString(const std::string& str, const char& ch)
		{
			std::deque<std::string> res;
			std::stringstream ss (str);
			std::string segment;
			
			while (std::getline(ss, segment, ch))
			{
				res.push_back(segment);
			}

			return res;
		}
		
		std::string GetRelativePath(const char* absolutePath)
		{
#if defined(WIN32)
			return std::string("../") + absolutePath;
#elif defined(__APPLE__)
			return absolutePath;
#else
			return absolutePath;
#endif
		}
		
	}
	
}
