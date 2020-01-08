#pragma once

#include "ecs/ISystem.h"
#include "ecs/components/CameraComponent.h"

namespace erm {
	namespace ecs {
		
		class TransformSystem;
		struct TransformComponent;
		
		class CameraSystem : public ISystem<CameraComponent>
		{
		public:
			CameraSystem(ECS& ecs);
			
			void OnUpdate(float dt);
			void OnPostUpdate();
			
		private:
			// ISystem
			void OnComponentAdded(EntityId /*id*/) override {}
			void OnComponentRemoved(EntityId /*id*/) override {}
			
			void UpdateCameraComponent(CameraComponent& camera, TransformComponent& transform);
			
			TransformSystem& mTransformSystem;
			
		};
		
	}
}

