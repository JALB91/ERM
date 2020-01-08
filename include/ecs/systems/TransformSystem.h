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
			
			void OnPostUpdate();
			
			void RemoveFromParent(EntityId id) const;
			void AddChild(EntityId parent, EntityId child) const;
			
		private:
			// ISystem
			void OnComponentBeingRemoved(EntityId id) override;
			
		};
		
	}
}
