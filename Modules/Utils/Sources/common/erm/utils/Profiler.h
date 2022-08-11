#pragma once

#include "erm/utils/Macros.h"
#include "erm/utils/Sampler.h"
#include "erm/utils/Timer.h"
#include "erm/utils/Tree.h"
#include "erm/utils/Utils.h"

#if defined(TRACY_ENABLE)
#	ifndef ERM_ZONE_COLOR
#	error ERM_ZONE_COLOR not defined
#	endif

#	include <Tracy.hpp>

namespace erm::internal {
constexpr const char* const kSimulationFrameName = "Sim";
constexpr const char* const kRenderFrameName = "Render";
}

#	define ERM_SIM_FRAME_BEGIN() FrameMarkStart(erm::internal::kSimulationFrameName)
#	define ERM_SIM_FRAME_END() FrameMarkEnd(erm::internal::kSimulationFrameName)
#	define ERM_RENDER_FRAME_BEGIN() FrameMarkStart(erm::internal::kRenderFrameName)
#	define ERM_RENDER_FRAME_END() FrameMarkEnd(erm::internal::kRenderFrameName)
#	define ERM_PROFILE(NAME) ZoneScopedNC(NAME, ERM_ZONE_COLOR)
#	define ERM_PROFILE_FUNCTION() ERM_PROFILE(ERM_FUNC_SIG)
#else
#	define ERM_SIM_FRAME_BEGIN() gSimTimer.Restart()
#	define ERM_SIM_FRAME_END() gSimTimer.Update(); gSimSampler.AddSample(gSimTimer.GetElapsedTime())
#	define ERM_RENDER_FRAME_BEGIN() gRenderTimer.Restart()
#	define ERM_RENDER_FRAME_END() gRenderTimer.Update(); gRenderSampler.AddSample(gRenderTimer.GetElapsedTime())
#	define ERM_PROFILE(NAME) erm::Profiler p(NAME)
#	define ERM_PROFILE_FUNCTION() ERM_PROFILE(Utils::StripFunctionName(ERM_FUNC_SIG))
#endif

namespace erm {

class Profiler
{
private:
	struct Profile
	{
		Profile(FrameID frameId)
			: mFrameId(frameId)
		{}
		
		Sampler mSampler;
		FrameID mFrameId;
	};
	
public:
	using ProfilingTree = Tree<std::string_view, Profile>;

public:
	Profiler(std::string_view id);
	~Profiler();

	static const ProfilingTree* GetProfilingTreeRoot();

private:
	Timer mTimer;
	
};

} // namespace erm
