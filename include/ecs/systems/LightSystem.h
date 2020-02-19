#pragma once

#include "ecs/components/LightComponent.h"
#include "ecs/ISystem.h"

namespace erm {
	namespace ecs {
		
		class LightSystem : public ISystem<LightComponent>
		{
		public:
			LightSystem(ECS& ecs)
				: ISystem(ecs)
			{}
			~LightSystem() = default;
		};
		
	}
}
