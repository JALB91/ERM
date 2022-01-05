#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/SkeletonComponent.h"

namespace erm::ecs {

class SkeletonSystem : public ISystem
{
	ERM_SYSTEM_DECL(Skeleton)

public:
	SkeletonSystem(Engine& engine)
		: ISystem(engine)
	{}

	// ISystem
	void OnPostUpdate() override;
};

} // namespace erm::ecs
