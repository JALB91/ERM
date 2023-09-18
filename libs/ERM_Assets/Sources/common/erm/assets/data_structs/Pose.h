#pragma once

#include "erm/math/Types.h"

namespace erm {

struct Pose
{
	vec3 mTranslation = vec3(0.0f);
	vec3 mScale = vec3(1.0f);
	quat mRotation = glm::identity<quat>();
};

} // namespace erm
