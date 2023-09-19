#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/AnimationComponent.h"

namespace erm::ecs {

class AnimationSystem : public ISystem
{
	ERM_SYSTEM_DECL(Animation)

public:
	AnimationSystem(ECS& ecs);

	// ISystem
	void update(float dt) override;
	void postUpdate() override;

private:
	float mFrameTime;
};

} // namespace erm::ecs
