#pragma once

#include "erm/ecs/ECS.h"

namespace erm::ecs {

template<typename T>
T* getParentComponent(ECS& ecs, EntityId id)
{
	auto* entity = ecs.getEntityById(id);
	if (entity == nullptr)
	{
		return nullptr;
	}

	auto* parent = ecs.getEntityById(entity->getParent());
	if (!parent)
	{
		return nullptr;
	}

	auto* system = ecs.getSystem<typename T::SYSTEM_TYPE>();
	return system->getComponent(parent->getId());
}

} // namespace erm::ecs
