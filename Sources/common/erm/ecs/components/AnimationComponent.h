#pragma once

#include "erm/ecs/IComponent.h"

namespace erm {
struct SkeletonAnimation;
namespace ecs {
class AnimationSystem;
}
} // namespace erm

namespace erm::ecs {

struct AnimationComponent : public IComponent
{
public:
	typedef AnimationSystem SYSTEM_TYPE;
	friend class AnimationSystem;

public:
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

} // namespace erm::ecs
