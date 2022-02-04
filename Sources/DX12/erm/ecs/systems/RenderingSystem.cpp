#include "erm/ecs/systems/RenderingSystem.h"

namespace erm::ecs {

RenderingSystem::RenderingSystem(Engine& engine)
	: ISystem(engine)
{}

ERM_SYSTEM_IMPL(Rendering)

} // namespace erm::ecs