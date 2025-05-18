#include "erm/ERM_ECS.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_ECSImpl::init()
{
	ObjectRegistry::set<ecs::ECS>(mECS);
	return true;
}

bool ERM_ECSImpl::deinit()
{
	ObjectRegistry::remove<ecs::ECS>();
	return true;
}

}
