#pragma once

#include "erm/ecs/EntityId.h"
#include "erm/ecs/ECSConfig.h"

#include <array>
#include <memory>
#include <utility>
#include <type_traits>

namespace erm {
	
	class Renderer;
	
	namespace ecs {
		
		class ECS;
		
		template<
			typename T,
			typename Enable = std::enable_if_t<std::is_default_constructible_v<T>>
		>
		class ISystem
		{
		public:
			ISystem(ECS& ecs)
				: mECS(ecs)
				, mComponents{}
				, mComponentsBitmask { false }
			{}
			virtual ~ISystem() = default;
			
			virtual void OnUpdate(float /*dt*/) {}
			virtual void OnPostUpdate() {}
			virtual void OnRender(const Renderer& /*renderer*/) {}
			
			virtual bool HasComponent(EntityId id) const final
			{
				return (id.IsValid() && mComponentsBitmask[id()]);
			}
			
			virtual const T* GetComponent(EntityId id) const final
			{
				return (HasComponent(id) ? &mComponents[id()] : nullptr);
			}
			
			virtual T* GetComponent(EntityId id) final
			{
				return (HasComponent(id) ? &mComponents[id()] : nullptr);
			}
			
			template<typename... Args>
			T* AddComponent(EntityId id, Args&&... args)
			{
				if (!HasComponent(id))
				{
					OnComponentBeingAdded(id);
					mComponentsBitmask[id()] = true;
					mComponents[id()] = T(std::forward<Args>(args)...);
					OnComponentAdded(id);
				}
				
				return GetComponent(id);
			}
			
			template<typename... Args>
			T* RequireComponent(EntityId id, Args&&... args)
			{
				return (HasComponent(id) ? GetComponent(id) : AddComponent(id, std::forward<Args>(args)...));
			}
			
			virtual void RemoveComponent(EntityId id) final
			{
				if (!HasComponent(id))
				{
					return;
				}
				
				OnComponentBeingRemoved(id);
				mComponentsBitmask[id()] = false;
				OnComponentRemoved(id);
			}
			
		protected:
			virtual void OnComponentBeingAdded(EntityId /*id*/) {}
			virtual void OnComponentAdded(EntityId /*id*/) {}
			
			virtual void OnComponentBeingRemoved(EntityId /*id*/) {}
			virtual void OnComponentRemoved(EntityId /*id*/) {}
			
			ECS& mECS;
			std::array<T, MAX_ID> mComponents;
			std::array<bool, MAX_ID> mComponentsBitmask;
			
		};
		
	}
}
