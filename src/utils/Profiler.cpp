#include "utils/Profiler.h"

#include "game/Game.h"

namespace {
	
	std::unordered_map<const char*, std::pair<double, bool>> profiling;
	
}

namespace erm {
	
	void Profiler::Profile(const char* id)
	{
		profiling[id] = std::make_pair(Timer::GetCurrentTime(), false);
	}
	
	void Profiler::EndProfiling(const char* id)
	{
		profiling[id] = std::make_pair(Timer::GetCurrentTime() - profiling[id].first, true);
	}
	
	const std::unordered_map<const char*, std::pair<double, bool>>& Profiler::GetProfilers()
	{
		return profiling;
	}
	
}
