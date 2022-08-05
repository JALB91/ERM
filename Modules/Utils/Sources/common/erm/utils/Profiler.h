#pragma once

#include "erm/utils/Timer.h"
#include "erm/utils/Tree.h"
#include "erm/utils/Utils.h"

#if defined(ERM_WINDOWS)
#	define ERM_FUNC_SIG __FUNCSIG__
#elif defined(ERM_MAC)
#	define ERM_FUNC_SIG __PRETTY_FUNCTION__
#endif

#if defined(TRACY_ENABLE)
#	include <Tracy.hpp>

#	define ERM_FRAME_MARK() FrameMark
#	define ERM_PROFILE(NAME) ZoneScoped
#	define ERM_PROFILE_FUNCTION() ZoneScoped
#elif !defined(NDEBUG)
#	define ERM_FRAME_MARK()
#	define ERM_PROFILE(NAME) erm::Profiler p(NAME)
#	define ERM_PROFILE_FUNCTION() ERM_PROFILE(Utils::StripFunctionName(ERM_FUNC_SIG))
#else
#	define ERM_FRAME_MARK()
#	define ERM_PROFILE(NAME)
#	define ERM_PROFILE_FUNCTION()
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
