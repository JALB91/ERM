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

	class CameraSystem : public ISystem<CameraComponent>
	{
	public:
		CameraSystem(ECS& ecs, const IWindow& window);

		void OnUpdate(float dt) override;
		void OnPostUpdate() override;

	private:
		void UpdateCameraComponent(CameraComponent& camera, TransformComponent& transform);

		TransformSystem& mTransformSystem;
		const IWindow& mWindow;
	};

} // namespace erm::ecs
