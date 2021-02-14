#pragma once

#include "erm/math/quat.h"
#include "erm/math/vec.h"

namespace erm {

	struct Pose
	{
		math::vec3 mTranslation;
		math::vec3 mScale;
		math::quat mRotation;
	};

} // namespace erm
