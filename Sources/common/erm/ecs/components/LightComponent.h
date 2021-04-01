#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/math/vec.h"

namespace erm::ecs {
class LightSystem;
}

namespace erm::ecs {

struct LightComponent : IComponent
{
public:
	typedef LightSystem SYSTEM_TYPE;
	friend class LightSystem;

	math::vec3 mAmbient {1.0f};
	math::vec3 mDiffuse {1.0f};
	math::vec3 mSpecular {1.0f};
};

} // namespace erm::ecs
