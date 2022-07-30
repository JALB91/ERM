#pragma once

#include "erm/math/quat.h"
#include "erm/math/vec.h"

namespace erm {

struct Pose
{
	math::vec3 mTranslation = math::vec3(0.0f);
	math::vec3 mScale = math::vec3(1.0f);
	math::quat mRotation = glm::identity<math::quat>();
};

} // namespace erm
