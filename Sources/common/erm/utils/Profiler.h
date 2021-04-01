#pragma once

#include "erm/utils/Timer.h"
#include "erm/utils/Tree.h"
#include "erm/utils/Utils.h"

#define PROFILE(name) erm::Profiler p(name)
#ifdef WIN32
#	define PROFILE_FUNCTION() PROFILE(Utils::StripFunctionName(__FUNCSIG__))
#else
#	define PROFILE_FUNCTION() PROFILE(Utils::StripFunctionName(__PRETTY_FUNCTION__))
#endif

namespace erm {

class Profiler
{
public:
	struct Profile
	{
		Profile(short frameId, double time = 0.0)
			: mFrameId(frameId)
			, mTime(time)
		{}

		short mFrameId;
		double mTime;
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

} // namespace erm
