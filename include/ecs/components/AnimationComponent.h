#pragma once

#include "ecs/IComponent.h"

#include "rendering/animations/SkeletonAnimation.h"

namespace erm {
	namespace ecs {
		
		class AnimationSystem;
		
		struct AnimationComponent : public IComponent
		{
			typedef AnimationSystem SYSTEM_TYPE;
			
			AnimationComponent(SkeletonAnimation skeletonAnimation)
				: mSkeletonAnimation(std::move(skeletonAnimation))
			{}
			
			SkeletonAnimation mSkeletonAnimation;
		};
		
	}
}
