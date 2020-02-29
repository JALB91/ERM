#pragma once

#include "ecs/IComponent.h"

namespace erm {
	
	struct SkeletonAnimation;
	
	namespace ecs {

		class AnimationSystem;
		
		struct AnimationComponent : public IComponent
		{
			typedef AnimationSystem SYSTEM_TYPE;
			
			AnimationComponent(SkeletonAnimation* skeletonAnimation)
				: mSkeletonAnimation(skeletonAnimation)
				, mCurrentAnimationTime(0.0f)
				, mPlaying(false)
			{}
			
			SkeletonAnimation* mSkeletonAnimation;
			float mCurrentAnimationTime;
			bool mPlaying;
		};
		
	}
}
