#include "erm/ERM_ECS.h"

#include "erm/ecs/ECS.h"

#include <erm/modules/ObjectRegistry.h>

namespace erm {

bool ERM_ECS::Impl::init() const
{
	ObjectRegistry::set<ecs::ECS>(std::make_shared<ecs::ECS>());
	return true;
}

bool ERM_ECS::Impl::deinit() const
{
	ObjectRegistry::remove<ecs::ECS>();
	return true;
}

}
