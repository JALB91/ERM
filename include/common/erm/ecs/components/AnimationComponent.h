#pragma once

#include "erm/ecs/IComponent.h"

namespace erm {
	
	struct SkeletonAnimation;
	
	namespace ecs {

		class AnimationSystem;
		
		struct AnimationComponent : public IComponent
		{
			typedef AnimationSystem SYSTEM_TYPE;
			
			AnimationComponent(SkeletonAnimation* skeletonAnimation = nullptr)
				: mSkeletonAnimation(skeletonAnimation)
				, mCurrentAnimationTime(0.0f)
				, mTimeScale(1.0f)
				, mPlaying(false)
			{}
			
			SkeletonAnimation* mSkeletonAnimation;
			float mCurrentAnimationTime;
			float mTimeScale;
			bool mPlaying;
		};
		
	}
}
