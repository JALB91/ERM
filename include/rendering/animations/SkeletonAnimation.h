#pragma once

#include "rendering/animations/KeyFrame.h"

#include <vector>

namespace erm {
	
	struct SkeletonAnimation
	{
		SkeletonAnimation(const std::vector<KeyFrame>& keyFrames, float totalAnimationTime)
			: mKeyFrames(keyFrames)
			, mTotalAnimationTime(totalAnimationTime)
		{}
		
		std::vector<KeyFrame> mKeyFrames;
		float mTotalAnimationTime;
	};
	
}
