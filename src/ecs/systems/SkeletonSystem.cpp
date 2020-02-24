#include "ecs/systems/SkeletonSystem.h"

namespace erm {
	namespace ecs {
		
		SkeletonSystem::SkeletonSystem(ECS& ecs)
			: ISystem(ecs)
		{};
		
		void SkeletonSystem::OnPostUpdate()
		{}
		
	}
}
