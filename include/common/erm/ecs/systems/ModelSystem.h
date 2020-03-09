#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/ModelComponent.h"

namespace erm {
	namespace ecs {
		
		class TransformSystem;
		
		class ModelSystem : public ISystem<ModelComponent>
		{
		public:
			ModelSystem(ECS& ecs);
			
			void OnPostUpdate() override;
			
		private:
			TransformSystem& mTransformSystem;
			
		};
		
	}
}
