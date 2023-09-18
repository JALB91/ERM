#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/LightComponent.h"

namespace erm::ecs {

class LightSystem : public ISystem
{
	ERM_SYSTEM_DECL(Light)

public:
	LightSystem(ECS& ecs)
		: ISystem(ecs)
	{}
	~LightSystem() = default;
};

} // namespace erm::ecs
