#pragma once

#include "ecs/ISystem.h"
#include "ecs/components/AnimationComponent.h"

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
