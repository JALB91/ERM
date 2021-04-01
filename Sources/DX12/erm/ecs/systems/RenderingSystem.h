#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

namespace erm::ecs {

class RenderingSystem : public ISystem<RenderingComponent>
{
public:
	RenderingSystem(ECS& ecs);

private:
};

} // namespace erm::ecs