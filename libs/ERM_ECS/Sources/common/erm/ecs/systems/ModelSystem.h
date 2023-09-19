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
	void init() override;
	void postUpdate() override;

private:
	// ISystem
	void onComponentBeingRemoved(EntityId id) override;

	TransformSystem* mTransformSystem;
	RenderingSystem* mRenderingSystem;
};

} // namespace erm::ecs
