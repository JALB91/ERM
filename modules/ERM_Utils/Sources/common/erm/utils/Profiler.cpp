#include "erm/utils/Profiler.h"

#include "erm/utils/Utils.h"

#include <erm/log/Assert.h>

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

	auto* node = sCurrentNode->find(id);
	if (!node)
	{
		node = &(sCurrentNode->addChild(id, Profile(Timer::sFrameId)));
	}
	sCurrentNode = node;
}

Profiler::~Profiler()
{
	ERM_ASSERT(sCurrentNode != nullptr);
	
	mTimer.update();
	
	Profile& profile = sCurrentNode->getPayload();
	const double time = mTimer.getElapsedTime();
	
	if (profile.mFrameId != Timer::sFrameId)
	{
		profile.mSampler.addSample(time);
	}
	else
	{
		profile.mSampler.addToCurrentSample(time);
	}
	
	profile.mFrameId = Timer::sFrameId;
	
	auto* parent = sCurrentNode->getParent();
	if (parent != nullptr)
	{
		sCurrentNode = parent;
	}
}

const Profiler::ProfilingTree* Profiler::getProfilingTreeRoot()
{
	return sTree.get();
}

} // namespace erm
