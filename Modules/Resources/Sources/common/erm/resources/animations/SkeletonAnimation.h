#pragma once

#include "erm/resources/animations/KeyFrame.h"

#include <string>
#include <string_view>
#include <vector>

namespace erm {

struct SkeletonAnimation
{
	SkeletonAnimation(
		const std::vector<KeyFrame>& keyFrames,
		float totalAnimationTime,
		std::string_view name)
		: mKeyFrames(keyFrames)
		, mTotalAnimationTime(totalAnimationTime)
		, mName(name)
	{}

	SkeletonAnimation(
		std::vector<KeyFrame>&& keyFrames,
		float totalAnimationTime,
		std::string_view name)
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
