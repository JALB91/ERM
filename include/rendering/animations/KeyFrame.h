#pragma once

#include "rendering/data_structs/Bone.h"
#include "rendering/data_structs/Pose.h"

#include <array>

namespace erm {
	
	struct KeyFrame
	{
		const float mTimestamp;
		const std::array<Pose, MAX_BONES> mTransforms;
	};
	
}
