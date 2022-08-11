#pragma once

#include "erm/utils/Macros.h"
#include "erm/utils/Timer.h"
#include "erm/utils/Tree.h"
#include "erm/utils/Utils.h"

#if defined(TRACY_ENABLE)
#	ifndef ERM_ZONE_COLOR
#	error ERM_ZONE_COLOR not defined
#	endif

#	include <Tracy.hpp>

#	define ERM_FRAME_BEGIN(NAME) FrameMarkStart(NAME)
#	define ERM_FRAME_END(NAME) FrameMarkEnd(NAME)
#	define ERM_PROFILE(NAME) ZoneScopedNC(NAME, ERM_ZONE_COLOR)
#	define ERM_PROFILE_FUNCTION() ERM_PROFILE(ERM_FUNC_SIG)
#else
#	define ERM_FRAME_BEGIN(NAME) ERM_UNUSED(NAME)
#	define ERM_FRAME_END(NAME) ERM_UNUSED(NAME)
#	define ERM_PROFILE(NAME) erm::Profiler p(NAME)
#	define ERM_PROFILE_FUNCTION() ERM_PROFILE(Utils::StripFunctionName(ERM_FUNC_SIG))
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

	static const ProfilingTree* GetRoot();

private:
	Timer mTimer;
	
};

} // namespace erm
