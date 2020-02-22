#pragma once

#include "utils/Timer.h"
#include "utils/Tree.h"
#include "utils/Utils.h"

#define PROFILE(name) erm::Profiler p(name)
#define PROFILE_FUNCTION() PROFILE(Utils::StripFunctionName(__PRETTY_FUNCTION__))

namespace erm {
	
	class Profiler
	{
	public:
		struct Profile
		{
			Profile(double time = 0.0, bool done = false)
				: mTime(time)
				, mDone(done)
			{}
			
			double mTime;
			bool mDone;
		};
		
		using ProfilingTree = Tree<std::string, Profile>;
		
	public:
		Profiler(const std::string& id);
		~Profiler();
		
		static const ProfilingTree& GetRoot();
		
	private:
		static std::unique_ptr<ProfilingTree> sTree;
		static ProfilingTree* sCurrentNode;
		
		const Timer mTimer;
		
	};
	
}
