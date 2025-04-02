#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

namespace erm::ecs {

class RenderingSystem : public ISystem
{
public:
	RenderingSystem(Engine& engine);

	ERM_SYSTEM_DECL(Rendering);
};

} // namespace erm::ecs