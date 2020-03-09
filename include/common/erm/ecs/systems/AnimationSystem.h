#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/AnimationComponent.h"

namespace erm {
	namespace ecs {
		
		class ECS;
		class SkeletonSystem;
		
		class AnimationSystem : public ISystem<AnimationComponent>
		{
		public:
			AnimationSystem(ECS& ecs);
			
			// ISystem
			void OnUpdate(float dt) override;
			void OnPostUpdate() override;
			
		private:
			SkeletonSystem& mSkeletonSystem;
			float mFrameTime;
			
		};
		
	}
}
