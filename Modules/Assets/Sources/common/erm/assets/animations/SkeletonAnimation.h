#pragma once

#include "erm/assets/animations/KeyFrame.h"

#include <vector>

namespace erm {

struct SkeletonAnimation
{
	SkeletonAnimation()
		: mTotalAnimationTime(0.0f)
	{}
	
	std::vector<KeyFrame> mKeyFrames;
	float mTotalAnimationTime;	
};

} // namespace erm
