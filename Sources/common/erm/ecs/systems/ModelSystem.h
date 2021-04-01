#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/ModelComponent.h"

namespace erm::ecs {
class TransformSystem;
}

namespace erm::ecs {

class ModelSystem : public ISystem<ModelComponent>
{
public:
	ModelSystem(ECS& ecs);

	// ISystem
	void Init() override;
	void OnPostUpdate() override;

private:
	// ISystem
	void OnComponentBeingRemoved(EntityId id) override;

	TransformSystem* mTransformSystem;
};

} // namespace erm::ecs
