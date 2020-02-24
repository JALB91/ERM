#pragma once

#include "ecs/ISystem.h"
#include "ecs/components/SkeletonComponent.h"

namespace erm {
	namespace ecs {
		
		class SkeletonSystem : public ISystem<SkeletonComponent>
		{
		public:
			SkeletonSystem(ECS& ecs);
			
			void OnPostUpdate() override;
			
		};
		
	}
}
