#pragma once

#include <assert.h>
#include <optional>
#include <string>
#include <vector>

#define UNUSED(x)	   (void)x;
#define ASSERT(x)	   assert(x)
#define EXPECT(x, msg) erm::Utils::LogCall((x), msg, #x, __FILE__, __LINE__)

#define DECL_SET_GET_OPT_WITH_DEFAULT(NAME, TYPE, DEFAULT)              \
private:                                                                \
	std::optional<TYPE> m##NAME;                                        \
                                                                        \
public:                                                                 \
	inline TYPE Get##NAME() const { return m##NAME.value_or(DEFAULT); } \
	inline void Set##NAME(const TYPE& v) { m##NAME = v; }               \
	inline bool Has##NAME##Value() const { return m##NAME.has_value(); }

namespace erm::Utils {

	extern bool LogCall(bool cond, const char* msg, const char* function, const char* file, int line);
	extern std::vector<std::string> SplitString(const std::string& str, char ch);
	extern std::string StripFunctionName(const char* fn);

} // namespace erm::Utils
