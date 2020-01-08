#pragma once

#include "ecs/EntityId.h"
#include "ecs/ECSConfig.h"

#include <array>
#include <memory>

namespace erm {
	namespace ecs {
		
		class ECS;
		
		template<typename T>
		class ISystem
		{
		public:
			ISystem(ECS& ecs)
				: mECS(ecs)
				, mComponents{ nullptr }
			{}
			virtual ~ISystem() = default;
			
			bool HasComponent(EntityId id) const
			{
				return (id.IsValid() && mComponents[id()].get());
			}
			
			T* GetComponent(EntityId id) const
			{
				return (HasComponent(id) ? mComponents[id()].get() : nullptr);
			}
			
			template<typename... Args>
			T* AddComponent(EntityId id, Args&&... args)
			{
				if (!HasComponent(id))
				{
					OnComponentBeingAdded(id);
					mComponents[id()] = std::make_unique<T>(std::move(args)...);
					OnComponentAdded(id);
				}
				
				return GetComponent(id);
			}
			
			template<typename... Args>
			T* RequireComponent(EntityId id, Args&&... args)
			{
				return (HasComponent(id) ? GetComponent(id) : AddComponent(id, std::move(args)...));
			}
			
			void RemoveComponent(EntityId id)
			{
				if (!HasComponent(id))
				{
					return;
				}
				
				OnComponentBeingRemoved(id);
				mComponents[id()].reset();
				OnComponentRemoved(id);
			}
			
		protected:
			virtual void OnComponentBeingAdded(EntityId /*id*/) {}
			virtual void OnComponentAdded(EntityId /*id*/) {}
			
			virtual void OnComponentBeingRemoved(EntityId /*id*/) {}
			virtual void OnComponentRemoved(EntityId /*id*/) {}
			
			ECS& mECS;
			std::array<std::unique_ptr<T>, MAX_ID> mComponents;
			
		};
		
	}
}
