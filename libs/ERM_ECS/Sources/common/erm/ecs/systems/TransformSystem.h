#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/TransformComponent.h"

namespace erm::ecs {

class TransformSystem : public ISystem
{
	ERM_SYSTEM_DECL(Transform)

public:
	TransformSystem(ECS& ecs);

private:
	// ISystem
	void OnPostUpdate() override;
	void OnEntityParentChanged(EntityId entityId) override;

	void UpdateDirtyRecursive(EntityId id);
};

} // namespace erm::ecs
