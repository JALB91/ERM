#pragma once

#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/Pose.h"

#include <array>

namespace erm {

	struct KeyFrame
	{
		KeyFrame(float timestamp)
			: mTimestamp(timestamp)
		{}

		std::array<Pose, MAX_BONES> mTransforms;
		float mTimestamp;
	};

} // namespace erm
