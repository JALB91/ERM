#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/CameraComponent.h"

namespace erm {
class IWindow;
namespace ecs {
class TransformSystem;
struct TransformComponent;
} // namespace ecs
} // namespace erm

namespace erm::ecs {

class CameraSystem : public ISystem
{
	ERM_SYSTEM_DECL(Camera)

public:
	CameraSystem(Engine& engine);

	// ISystem
	void Init() override;
	void OnUpdate(float dt) override;
	void OnPostUpdate() override;

private:
	void UpdateCameraComponent(CameraComponent& camera, TransformComponent& transform, float dt);

	TransformSystem* mTransformSystem;
	const IWindow& mWindow;
};

} // namespace erm::ecs
