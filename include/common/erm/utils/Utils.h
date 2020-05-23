#pragma once

#include <assert.h>
#include <string>
#include <vector>

#define UNUSED(x)	   (void)x;
#define ASSERT(x)	   assert(x)
#define EXPECT(x, msg) erm::Utils::LogCall((x), msg, #x, __FILE__, __LINE__)

namespace erm::Utils {

	extern bool LogCall(bool cond, const char* msg, const char* function, const char* file, int line);
	extern std::vector<std::string> SplitString(const std::string& str, char ch);
	extern std::string StripFunctionName(const char* fn);

} // namespace erm::Utils
