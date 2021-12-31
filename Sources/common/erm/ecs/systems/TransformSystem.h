#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/TransformComponent.h"

namespace erm::ecs {

class TransformSystem : public ISystem
{
	ERM_SYSTEM_DECL(Transform)

public:
	TransformSystem(Engine& engine);

	// ISystem
	void Init() override {}
	void OnPostUpdate() override;

	void RemoveFromParent(EntityId id);
	void AddChild(EntityId parent, EntityId child);

private:
	// ISystem
	void OnComponentAdded(EntityId id) override;
	void OnComponentBeingRemoved(EntityId id) override;

	void UpdateDirtyRecursive(EntityId id);
};

} // namespace erm::ecs
