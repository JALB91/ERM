#pragma once

#include "erm/assets/animations/KeyFrame.h"

#include <vector>

namespace erm {

struct SkeletonAnimation
{
	std::vector<KeyFrame> mKeyFrames;
	float mTotalAnimationTime = 0.0f;
};

} // namespace erm
