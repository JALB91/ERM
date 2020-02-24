#pragma once

#include "ecs/IComponent.h"

#include "rendering/animations/SkeletonAnimation.h"

namespace erm {
	namespace ecs {
		
		class SkeletonSystem;
		
		struct AnimationComponent : public IComponent
		{
			typedef SkeletonSystem SYSTEM_TYPE;
			
			SkeletonAnimation mSkeletonAnimation;
		};
		
	}
}
