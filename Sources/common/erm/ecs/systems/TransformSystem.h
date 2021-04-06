#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/TransformComponent.h"

namespace erm::ecs {

class TransformSystem : public ISystem<TransformComponent>
{
public:
	TransformSystem(ECS& ecs);

	// ISystem
	void Init() override {}
	void OnPostUpdate() override;

	void RemoveFromParent(EntityId id);
	void AddChild(EntityId parent, EntityId child);

private:
	// ISystem
	void OnComponentAdded(EntityId id) override;
	void OnComponentBeingRemoved(EntityId id) override;
};

} // namespace erm::ecs
