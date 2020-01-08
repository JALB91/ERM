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
			void OnComponentAdded(ID /*id*/) override {}
			void OnComponentRemoved(ID /*id*/) override {}
			
			void UpdateCameraComponent(CameraComponent& camera, TransformComponent& transform);
			
			TransformSystem& mTransformSystem;
			
		};
		
	}
}

