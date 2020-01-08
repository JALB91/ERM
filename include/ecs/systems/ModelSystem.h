#pragma once

#include "ecs/ISystem.h"
#include "ecs/components/ModelComponent.h"

namespace erm {
	namespace ecs {
		
		class TransformSystem;
		
		class ModelSystem : public ISystem<ModelComponent>
		{
		public:
			ModelSystem(ECS& ecs);
			
			void OnPostUpdate();
			
		private:
			// ISystem
			void OnComponentAdded(ID /*id*/) override {}
			void OnComponentRemoved(ID /*id*/) override {}
			
			TransformSystem& mTransformSystem;
			
		};
		
	}
}
