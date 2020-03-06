#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/math/vec.h"

namespace erm {
	namespace ecs {
		
		class LightSystem;
		
		struct LightComponent : IComponent
		{
			typedef LightSystem SYSTEM_TYPE;
			
			math::vec3 mAmbient { 1.0f };
			math::vec3 mDiffuse { 1.0f };
			math::vec3 mSpecular { 1.0f };
		};
		
	}
}
