#pragma once

#include <tuple>
#include <unordered_map>

#define PROFILE(x, NAME) \
	Profiler::Profile(NAME); \
	x; \
	Profiler::EndProfiling(NAME);

namespace erm {
	
	class Profiler
	{
	public:
		static void Profile(const char* id);
		static void EndProfiling(const char* id);
		
		static const std::unordered_map<const char*, std::pair<double, bool>>& GetProfilers();
		
	};
	
}
