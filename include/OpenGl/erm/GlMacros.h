#pragma once

#include "erm/utils/Utils.h"
#include <GL/glew.h>

#include <iostream>

#define GL_CALL(x) \
	erm::GLClearError();\
	x;\
	ASSERT(erm::GLLogCall(#x, __FILE__, __LINE__))

namespace erm {
	
	static bool GLLogCall(const char* function, const char* file, int line)
	{
		while (GLenum error = glGetError())
		{
			std::cout << "[OpenGL error] (" << error << ") " << function << " " << file << ":" << line << std::endl;
			return false;
		}
		return true;
	}
	
	static void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}
	
}
