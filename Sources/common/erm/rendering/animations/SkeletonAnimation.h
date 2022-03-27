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
		const char* name)
		: mKeyFrames(keyFrames)
		, mTotalAnimationTime(totalAnimationTime)
		, mName(name)
	{}

	SkeletonAnimation(
		std::vector<KeyFrame>&& keyFrames,
		float totalAnimationTime,
		const char* name)
		: mKeyFrames(std::move(keyFrames))
		, mTotalAnimationTime(totalAnimationTime)
		, mName(name)
	{}

	SkeletonAnimation()
		: mTotalAnimationTime(0.0f)
		, mName("Unknown")
	{}

	std::vector<KeyFrame> mKeyFrames;
	float mTotalAnimationTime;
	std::string mName;
};

} // namespace erm
