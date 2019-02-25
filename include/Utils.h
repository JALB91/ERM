#pragma once

#include <string>
#include <vector>
#include <assert.h>

#define ASSERT(x) assert(x)
#define GLCALL(x) \
	erm::GLClearError();\
	x;\
	ASSERT(erm::GLLogCall(#x, __FILE__, __LINE__))

namespace erm {
	
	void GLClearError();
	bool GLLogCall(const char* function, const char* file, int line);
	
	std::vector<std::string> SplitString(const std::string& str, char ch);
	
}
