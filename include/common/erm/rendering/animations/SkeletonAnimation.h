#pragma once

#include "erm/rendering/animations/KeyFrame.h"

#include <string>
#include <vector>

namespace erm {
	
	struct SkeletonAnimation
	{
		SkeletonAnimation(
			const std::vector<KeyFrame>& keyFrames,
			float totalAnimationTime,
			const char* name
			)
			: mKeyFrames(keyFrames)
			, mTotalAnimationTime(totalAnimationTime)
			, mName(name)
		{}
		
		std::vector<KeyFrame> mKeyFrames;
		float mTotalAnimationTime;
		std::string mName;
	};
	
}
