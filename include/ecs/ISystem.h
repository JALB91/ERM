#pragma once

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
			{}
			virtual ~ISystem() = default;
			
			bool HasComponent(ID id) const
			{
				return mComponents[id].get();
			}
			
			T* GetComponent(ID id) const
			{
				return mComponents[id].get();
			}
			
			template<typename... Args>
			T* AddComponent(ID id, Args&&... args)
			{
				if (!HasComponent(id))
				{
					mComponents[id] = std::make_unique<T>(args...);
					OnComponentAdded(id);
				}
				
				return GetComponent(id);
			}
			
			template<typename... Args>
			T* RequireComponent(ID id, Args&&... args)
			{
				return (HasComponent(id) ? GetComponent(id) : AddComponent(id, std::move(args)...));
			}
			
			void RemoveComponent(ID id)
			{
				if (!HasComponent(id))
				{
					return;
				}
				
				mComponents[id].reset();
				OnComponentRemoved(id);
			}
			
		protected:
			virtual void OnComponentAdded(ID id) = 0;
			virtual void OnComponentRemoved(ID id) = 0;
			
			template<typename F>
			void ForEachComponent(const F& function)
			{
				for (ID i = 0; i < MAX_ENTITIES; ++i)
				{
					if (HasComponent(i))
					{
						function(GetComponent(i));
					}
				}
			}
			
			ECS& mECS;
			std::array<std::unique_ptr<T>, MAX_ENTITIES> mComponents;
			
		};
		
	}
}
