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
	void postUpdate() override;
	void onEntityParentChanged(EntityId entityId) override;

	void updateDirtyRecursive(EntityId id);
};

} // namespace erm::ecs
