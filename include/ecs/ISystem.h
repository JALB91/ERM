#pragma once

#include "ecs/EntityId.h"
#include "ecs/ECSConfig.h"

#include <array>
#include <memory>

namespace erm {
	
	class Renderer;
	
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
			
			virtual void OnUpdate(float /*dt*/) {}
			virtual void OnPostUpdate() {}
			virtual void OnRender(const Renderer& /*renderer*/) {}
			
			virtual bool HasComponent(EntityId id) const final
			{
				return (id.IsValid() && mComponents[id()].get());
			}
			
			virtual T* GetComponent(EntityId id) const final
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
			
			virtual void RemoveComponent(EntityId id) final
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
