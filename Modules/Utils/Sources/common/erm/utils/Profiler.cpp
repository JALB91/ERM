#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"

namespace erm {

static std::unique_ptr<Profiler::ProfilingTree> sTree = nullptr;
static Profiler::ProfilingTree* sCurrentNode = nullptr;

Profiler::Profiler(std::string_view id)
{
	if (!sTree)
	{
		sTree = std::make_unique<ProfilingTree>(id, Profile(Timer::sFrameId));
		sCurrentNode = sTree.get();
		return;
	}

	ProfilingTree* node = sCurrentNode->Find(id);
	if (!node)
	{
		node = &(sCurrentNode->AddChild(id, Profile(Timer::sFrameId)));
	}
	sCurrentNode = node;
}

Profiler::~Profiler()
{
	ERM_ASSERT(sCurrentNode != nullptr);
	
	mTimer.Update();
	
	Profile& profile = sCurrentNode->GetPayload();
	const double time = mTimer.GetElapsedTime();
	
	if (profile.mFrameId != Timer::sFrameId)
		profile.mSampler.AddSample(time);
	else
		profile.mSampler.AddToCurrentSample(time);
	
	profile.mFrameId = Timer::sFrameId;
	
	if (ProfilingTree* parent = sCurrentNode->GetParent())
		sCurrentNode = parent;
}

const Profiler::ProfilingTree* Profiler::GetProfilingTreeRoot()
{
	return sTree.get();
}

} // namespace erm
