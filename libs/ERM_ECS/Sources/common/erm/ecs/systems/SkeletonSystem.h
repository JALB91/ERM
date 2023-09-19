#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/SkeletonComponent.h"

namespace erm::ecs {

class SkeletonSystem : public ISystem
{
	ERM_SYSTEM_DECL(Skeleton)

public:
	SkeletonSystem(ECS& ecs)
		: ISystem(ecs)
	{}

	// ISystem
	void postUpdate() override;
};

} // namespace erm::ecs
