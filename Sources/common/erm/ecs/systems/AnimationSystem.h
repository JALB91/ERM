#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/AnimationComponent.h"

namespace erm::ecs {
class ECS;
class SkeletonSystem;
} // namespace erm::ecs

namespace erm::ecs {

class AnimationSystem : public ISystem
{
	ERM_SYSTEM_DECL(Animation)

public:
	AnimationSystem(Engine& engine);

	// ISystem
	void Init() override;
	void OnUpdate(float dt) override;
	void OnPostUpdate() override;

private:
	SkeletonSystem* mSkeletonSystem;
	float mFrameTime;
};

} // namespace erm::ecs
