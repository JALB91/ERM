#pragma once

#include "math/quat.h"
#include "math/vec.h"

namespace erm {
	
	struct Pose
	{
		math::vec3 mTranslation;
		math::vec3 mScale;
		math::quat mRotation;
	};
	
}
