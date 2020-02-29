#pragma once

#include "ecs/components/TransformComponent.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/ECSConfig.h"
#include "ecs/ECS.h"

#include <string>
#include <utility>

namespace erm {
	namespace ecs {
		
		struct Entity
		{
		public:
			friend class ECS;
			
		public:
			~Entity()
			{
				mECS.OnEntityBeingRemoved(mId);
			}
			
			Entity(Entity&&) = delete;
			Entity(const Entity&) = delete;
			
			Entity& operator=(Entity&&) = delete;
			Entity& operator=(const Entity&) = delete;
			
			bool operator==(const Entity& other) const
			{
				return other.mId == mId;
			}
			
			inline EntityId GetId() const { return mId; }
			inline bool IsValid() const { return mId.IsValid(); }
			
			inline ECS& GetECS() const { return mECS; }
			
			inline const std::string& GetName() const { return mName; }
			inline void SetName(const std::string& name) { mName = name; }
			
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
				return mECS.GetSystem<typename T::SYSTEM_TYPE>().AddComponent(mId, std::forward<Args>(args)...);
			}
			
			template<typename T, typename... Args>
			inline T* RequireComponent(Args&&... args) const
			{
				return mECS.GetSystem<typename T::SYSTEM_TYPE>().RequireComponent(mId, std::forward<Args>(args)...);
			}
			
			inline EntityId GetParent() const { return mTransformComponent.GetParent(); }
			inline const std::vector<EntityId>& GetChildren() const { return mTransformComponent.GetChildren(); }
			
			inline void RemoveFromParent() { mTransformSystem.RemoveFromParent(mId); }
			inline void AddChild(Entity& child) { mTransformSystem.AddChild(mId, child.mId); }
			
		private:
			inline Entity(EntityId id, ECS& ecs, const std::string& name)
				: mId(id)
				, mECS(ecs)
				, mName(name)
				, mTransformSystem(mECS.GetSystem<TransformSystem>())
				, mTransformComponent(*RequireComponent<TransformComponent>())
			{}
			
			const EntityId mId;
			ECS& mECS;
			std::string mName;
			TransformSystem& mTransformSystem;
			TransformComponent& mTransformComponent;
			
		};
		
	}
}
