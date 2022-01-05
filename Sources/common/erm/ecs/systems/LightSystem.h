#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/LightComponent.h"

namespace erm::ecs {

class LightSystem : public ISystem
{
	ERM_SYSTEM_DECL(Light)

public:
	LightSystem(Engine& engine)
		: ISystem(engine)
	{}
	~LightSystem() = default;
};

} // namespace erm::ecs
