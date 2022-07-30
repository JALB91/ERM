#pragma once

#include "erm/ecs/Component.h"

namespace erm {
struct SkeletonAnimation;
namespace ecs {
class AnimationSystem;
}
} // namespace erm

namespace erm::ecs {

struct AnimationComponent
{
	ERM_COMPONENT_DECL(Animation)

public:
	SkeletonAnimation* mSkeletonAnimation = nullptr;
	float mCurrentAnimationTime = 0.0f;
	float mTimeScale = 1.0f;
	bool mPlaying = false;
};

} // namespace erm::ecs
