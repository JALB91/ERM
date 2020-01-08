#include "ecs/systems/TransformSystem.h"

#include "ecs/Entity.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	namespace ecs {
		
		TransformSystem::TransformSystem(ECS& ecs)
			: ISystem<TransformComponent>(ecs)
		{}
		
		void TransformSystem::OnComponentAdded(ID id)
		{
			std::array<bool, MAX_ENTITIES> temp { false };
			mTree[id] = std::make_pair(INVALID_ID, std::move(temp));
		}
		
		void TransformSystem::OnComponentRemoved(ID id)
		{
			if (Entity* entity = mECS.GetEntityById(id))
			{
				entity->RemoveFromParent();
				
				for (Entity* entity : entity->GetChildren())
				{
					entity->RemoveFromParent();
				}
			}
			
			auto it = mTree.find(id);
			if (it != mTree.end())
			{
				mTree.erase(it);
			}
		}
		
		void TransformSystem::OnPostUpdate()
		{
			for (ID i = 0; i < MAX_ENTITIES; ++i)
			{
				TransformComponent* transform = GetComponent(i);
				
				if (!transform || !transform->IsDirty()) continue;
				
				transform->mWorldTransform = glm::identity<math::mat4>();
				transform->mLocalTransform = glm::identity<math::mat4>();
				
				ID parent = GetParent(i);
				
				if (parent != INVALID_ID)
				{
					if (TransformComponent* parentTransform = GetComponent(parent))
					{
						transform->mWorldTransform = parentTransform->mWorldTransform;
					}
				}
				
				transform->mLocalTransform = glm::translate(transform->mLocalTransform, transform->mTranslation);
				transform->mLocalTransform = glm::rotate(transform->mLocalTransform, transform->mRotation.z, math::vec3(0.0f, 0.0f, 1.0f));
				transform->mLocalTransform = glm::rotate(transform->mLocalTransform, transform->mRotation.y, math::vec3(0.0f, 1.0f, 0.0f));
				transform->mLocalTransform = glm::rotate(transform->mLocalTransform, transform->mRotation.x, math::vec3(1.0f, 0.0f, 0.0f));
				transform->mLocalTransform = glm::scale(transform->mLocalTransform, transform->mScale);
				
				transform->mWorldTransform *= transform->mLocalTransform;
				
				transform->SetDirty(false);
			}
		}
		
		Entity* TransformSystem::GetParent(const Entity& entity) const
		{
			return (entity ? mECS.GetEntityById(mTree.at(entity.GetID()).first) : nullptr);
		}
		
		std::vector<Entity*> TransformSystem::GetChildren(const Entity& entity) const
		{
			if (entity)
			{
				return mECS.GetEntitiesByIds(mTree.at(entity.GetID()).second);
			}
			return {};
		}
		
		void TransformSystem::RemoveFromParent(const Entity& entity)
		{
			if (!entity) return;
			
			Entity* parent = GetParent(entity);
			
			if (!parent || !(*parent)) return;
			
			mTree.at(parent->GetID()).second[entity.GetID()] = false;
			mTree.at(entity.GetID()).first = INVALID_ID;
		}
		
		void TransformSystem::AddChild(const Entity& parent, const Entity& child)
		{
			if (!parent || !child || parent == child || &child == mECS.GetRoot()) return;
			RemoveFromParent(child);
			mTree.at(parent.GetID()).second[child.GetID()] = true;
			mTree.at(child.GetID()).first = parent.GetID();
		}
		
		ID TransformSystem::GetParent(ID id) const
		{
			return (id != INVALID_ID ? mTree.at(id).first : INVALID_ID);
		}
		
	}
}
