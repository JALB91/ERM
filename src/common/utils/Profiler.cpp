#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"

namespace erm {

	std::unique_ptr<Profiler::ProfilingTree> Profiler::sTree = nullptr;
	Profiler::ProfilingTree* Profiler::sCurrentNode = nullptr;

	Profiler::Profiler(const std::string& id)
	{
		if (!sTree)
		{
			sTree = std::make_unique<ProfilingTree>(id, Profile());
			sCurrentNode = sTree.get();
			return;
		}

		ProfilingTree* node = ProfilingTree::Find(*sCurrentNode, id);
		if (node)
		{
			node->GetPayload().mDone = false;
		}
		else
		{
			node = &(sCurrentNode->AddChild(id, Profile()));
		}
		sCurrentNode = node;
	}

	Profiler::~Profiler()
	{
		const double prevTime = sCurrentNode->GetPayload().mTime;
		const double time = mTimer.GetElapsedTime();
		sCurrentNode->GetPayload().mTime = prevTime == 0.0 ? time : (time + prevTime) / 2.0;
		sCurrentNode->GetPayload().mDone = true;
		if (ProfilingTree* parent = sCurrentNode->GetParent())
		{
			sCurrentNode = parent;
		}
	}

	const Profiler::ProfilingTree& Profiler::GetRoot()
	{
		return *sTree;
	}

} // namespace erm
