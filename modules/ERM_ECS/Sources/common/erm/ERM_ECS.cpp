#include "erm/ERM_ECS.h"

#include "erm/ecs/ECS.h"

#include <erm/utils/ObjectRegistry.h>

namespace erm {

void ERM_ECS::init()
{
	ObjectRegistry::set<ecs::ECS>(std::make_shared<ecs::ECS>());
}

}
