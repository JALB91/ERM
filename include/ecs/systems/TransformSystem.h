#pragma once

#include "ecs/ISystem.h"
#include "ecs/components/TransformComponent.h"

#include <map>
#include <array>
#include <vector>
#include <utility>

namespace erm {
	namespace ecs {
		
		struct Entity;
		
		class TransformSystem : public ISystem<TransformComponent>
		{
		public:
			TransformSystem(ECS& ecs);
			
			void OnPostUpdate();
			
			Entity* GetParent(const Entity& entity) const;
			std::vector<Entity*> GetChildren(const Entity& entity) const;
			
			void RemoveFromParent(const Entity& entity);
			void AddChild(const Entity& parent, const Entity& child);
			
		private:
			// ISystem
			void OnComponentAdded(ID id) override;
			void OnComponentRemoved(ID id) override;
			
			ID GetParent(ID id) const;
			
			std::map<ID, std::pair<ID, std::array<bool, MAX_ENTITIES>>> mTree;
			
		};
		
	}
}
