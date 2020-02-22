#pragma once

#include <vector>
#include <string>
#include <assert.h>

#define UNUSED(x) (void)x;
#define ASSERT(x) assert(x)
#define EXPECT(x, msg) erm::Utils::LogCall((x), msg, #x, __FILE__, __LINE__)
#define GL_CALL(x) \
	erm::Utils::GLClearError();\
	x;\
	ASSERT(erm::Utils::GLLogCall(#x, __FILE__, __LINE__))

namespace erm {
	
	namespace Utils {
	
		extern bool LogCall(bool cond, const char* msg, const char* function, const char* file, int line);
		
		extern bool GLLogCall(const char* function, const char* file, int line);
		extern void GLClearError();
		
		extern std::vector<std::string> SplitString(const std::string& str, char ch);
		extern std::string GetRelativePath(const char* absolutePath);
		
		extern std::string StripFunctionName(const char* fn);
		
	}
	
}
