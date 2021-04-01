#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/LightComponent.h"

namespace erm::ecs {

class LightSystem : public ISystem<LightComponent>
{
public:
	LightSystem(ECS& ecs)
		: ISystem(ecs)
	{}
	~LightSystem() = default;

	// ISystem
	void Init() override {}
};

} // namespace erm::ecs
