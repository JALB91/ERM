#pragma once

#include "erm/ecs/ECS.h"

namespace erm::ecs {

template<typename T>
T* GetParentComponent(ECS& ecs, EntityId id)
{
	auto* entity = ecs.GetEntityById(id);
	if (!entity)
		return nullptr;

	auto* parent = ecs.GetEntityById(entity->GetParent());
	if (!parent)
		return nullptr;

	auto* system = ecs.GetSystem<typename T::SYSTEM_TYPE>();
	return system->GetComponent(parent->GetId());
}

} // namespace erm::ecs
