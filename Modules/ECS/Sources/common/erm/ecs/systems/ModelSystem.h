#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/ModelComponent.h"

namespace erm::ecs {
class TransformSystem;
class RenderingSystem;
} // namespace erm::ecs

namespace erm::ecs {

class ModelSystem : public ISystem
{
	ERM_SYSTEM_DECL(Model)

public:
	ModelSystem(ECS& ecs);

	// ISystem
	void Init() override;
	void OnPostUpdate() override;

private:
	// ISystem
	void OnComponentBeingRemoved(EntityId id) override;

	TransformSystem* mTransformSystem;
	RenderingSystem* mRenderingSystem;
};

} // namespace erm::ecs
