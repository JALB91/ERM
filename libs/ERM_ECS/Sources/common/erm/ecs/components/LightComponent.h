#pragma once

#include "erm/ecs/Component.h"

#include "erm/math/Types.h"

namespace erm::ecs {
class LightSystem;
}

namespace erm::ecs {

struct LightComponent
{
	ERM_COMPONENT_DECL(Light)

public:
	vec3 mAmbient {1.0f};
	vec3 mDiffuse {1.0f};
	vec3 mSpecular {1.0f};
};

} // namespace erm::ecs
