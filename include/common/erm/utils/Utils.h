#pragma once

#include <vector>
#include <string>
#include <assert.h>

#define UNUSED(x) (void)x;
#define ASSERT(x) assert(x)
#define EXPECT(x, msg) erm::Utils::LogCall((x), msg, #x, __FILE__, __LINE__)

namespace erm {
	
	namespace Utils {
		
		extern std::vector<std::string> SplitString(const std::string& str, char ch);
		extern std::string StripFunctionName(const char* fn);
		
	}
	
}
