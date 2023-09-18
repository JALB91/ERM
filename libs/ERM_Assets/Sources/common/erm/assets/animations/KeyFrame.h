#pragma once

#include "erm/assets/data_structs/Bone.h"
#include "erm/assets/data_structs/Pose.h"

#include <array>

namespace erm {

struct KeyFrame
{
	KeyFrame(float timestamp)
		: mTransforms()
		, mTimestamp(timestamp)
	{}

	std::array<Pose, MAX_BONES> mTransforms;
	float mTimestamp;
};

} // namespace erm
