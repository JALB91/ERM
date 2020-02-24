#pragma once

#include "rendering/animations/KeyFrame.h"

#include <vector>

namespace erm {
	
	struct SkeletonAnimation
	{
		const float mTotalAnimationTime;
		float mCurrentAnimationTime;
		std::vector<KeyFrame> mKeyFrames;
	};
	
}
