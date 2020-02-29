#pragma once

#include "ecs/ISystem.h"
#include "ecs/EntityId.h"
#include "ecs/components/TransformComponent.h"

namespace erm {
	namespace ecs {
		
		class TransformSystem : public ISystem<TransformComponent>
		{
		public:
			TransformSystem(ECS& ecs);
			
			void OnPostUpdate() override;
			
			void RemoveFromParent(EntityId id);
			void AddChild(EntityId parent, EntityId child);
			
		private:
			// ISystem
			void OnComponentBeingRemoved(EntityId id) override;
			
		};
		
	}
}
