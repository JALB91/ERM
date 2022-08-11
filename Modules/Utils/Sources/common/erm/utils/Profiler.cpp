#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"

namespace erm {

static std::unique_ptr<Profiler::ProfilingTree> sTree = nullptr;
static Profiler::ProfilingTree* sCurrentNode = nullptr;

Profiler::Profiler(const std::string& id)
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
	const double time = mTimer.GetElapsedTime();
	
	if (Timer::sFrameId != sCurrentNode->GetPayload().mFrameId)
		sCurrentNode->GetPayload().mTime = time;
	else
		sCurrentNode->GetPayload().mTime += time;
	
	sCurrentNode->GetPayload().mFrameId = Timer::sFrameId;
	
	if (ProfilingTree* parent = sCurrentNode->GetParent())
		sCurrentNode = parent;
}

const Profiler::ProfilingTree* Profiler::GetRoot()
{
	return sTree.get();
}

} // namespace erm
