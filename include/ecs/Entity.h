#pragma once

#include "ecs/components/TransformComponent.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/ECSConfig.h"
#include "ecs/ECS.h"

#include <functional>
#include <utility>

namespace erm {
	namespace ecs {
		
		struct Entity
		{
		public:
			friend class ECS;
			
		public:
			explicit operator bool() const
			{
				return IsValid();
			}
			
			bool operator==(const Entity& other) const
			{
				return other.mId == mId;
			}
			
			inline bool IsValid() const { return (mId != INVALID_ID); }
			
			inline ID GetID() const { return mId; }
			inline ECS& GetECS() const { return mECS; }
			
			template<typename T>
			inline bool HasComponent() const
			{
				return mECS.GetSystem<typename T::SYSTEM_TYPE>().HasComponent(mId);
			}
			
			template<typename T>
			inline T* GetComponent() const
			{
				return mECS.GetSystem<typename T::SYSTEM_TYPE>().GetComponent(mId);
			}
			
			template<typename T, typename... Args>
			inline T* AddComponent(Args&&... args) const
			{
				return mECS.GetSystem<typename T::SYSTEM_TYPE>().AddComponent(mId, std::move(args)...);
			}
			
			template<typename T, typename... Args>
			inline T* RequireComponent(Args&&... args) const
			{
				return mECS.GetSystem<typename T::SYSTEM_TYPE>().RequireComponent(mId, std::move(args)...);
			}
			
			inline Entity* GetParent() const { return mTransformSystem.GetParent(*this); }
			inline std::vector<Entity*> GetChildren() const { return mTransformSystem.GetChildren(*this); }
			
			inline void RemoveFromParent() const { mTransformSystem.RemoveFromParent(*this); }
			inline void AddChild(const Entity& child) const { mTransformSystem.AddChild(*this, child); }
			
		private:
			inline Entity(ID id, ECS& ecs)
				: mId(id)
				, mECS(ecs)
				, mTransformSystem(mECS.GetSystem<TransformSystem>())
			{
				RequireComponent<TransformComponent>();
			}
			
			ID mId;
			ECS& mECS;
			TransformSystem& mTransformSystem;
			
		};
		
	}
}
