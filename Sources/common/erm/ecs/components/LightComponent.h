#pragma once

#include "erm/ecs/Component.h"

#include "erm/math/vec.h"

namespace erm::ecs {
class LightSystem;
}

namespace erm::ecs {

struct LightComponent
{
	ERM_COMPONENT_DECL(Light)

public:
	math::vec3 mAmbient {1.0f};
	math::vec3 mDiffuse {1.0f};
	math::vec3 mSpecular {1.0f};
};

} // namespace erm::ecs
