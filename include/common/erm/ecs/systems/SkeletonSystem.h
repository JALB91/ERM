#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/SkeletonComponent.h"

namespace erm::ecs {
	
	class SkeletonSystem : public ISystem<SkeletonComponent>
	{
	public:
		SkeletonSystem(ECS& ecs)
			: ISystem(ecs)
		{}
		
		void OnPostUpdate() override;
		
	};
	
}
