#include "ecs/systems/TransformSystem.h"

#include "ecs/Entity.h"

#include "utils/Profiler.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	namespace ecs {
		
		TransformSystem::TransformSystem(ECS& ecs)
			: ISystem<TransformComponent>(ecs)
		{}
		
		void TransformSystem::OnComponentBeingRemoved(EntityId id)
		{
			TransformComponent* transform = GetComponent(id);
			
			for (EntityId entityId : transform->mChildren)
			{
				TransformComponent* entityTransform = GetComponent(entityId);
				entityTransform->mParent.Reset();
				entityTransform->SetDirty(true);
			}
			transform->mChildren.clear();
			
			RemoveFromParent(id);
		}
		
		void TransformSystem::OnPostUpdate()
		{
			PROFILE_FUNCTION();
			
			for (ID i = ROOT_ID; i < MAX_ID; ++i)
			{
				TransformComponent* transform = GetComponent(i);
				
				if (!transform) continue;
				
				transform->mWorldTransform = glm::identity<math::mat4>();
				transform->mLocalTransform = glm::identity<math::mat4>();
				
				EntityId parent = transform->mParent;
				
				if (parent.IsValid())
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
		
		void TransformSystem::RemoveFromParent(EntityId id)
		{
			if (!id.IsValid()) return;
			
			TransformComponent* entityTransform = GetComponent(id);
			EntityId parentId = entityTransform->mParent;
			
			if (!parentId.IsValid()) return;
			
			std::vector<EntityId>& children = GetComponent(parentId)->mChildren;
			children.erase(std::find(children.begin(), children.end(), id));
			
			entityTransform->mParent.Reset();
			entityTransform->SetDirty(true);
		}
		
		void TransformSystem::AddChild(EntityId parent, EntityId child)
		{
			if (!parent.IsValid()
				|| !child.IsValid()
				|| parent == child
				|| child() == ROOT_ID
			) return;
			
			TransformComponent* parentTransform = GetComponent(parent);
			TransformComponent* childTransform = GetComponent(child);
			
			if (childTransform->mParent == parent) return;
			
			RemoveFromParent(child);
			parentTransform->mChildren.emplace_back(child);
			childTransform->mParent = parent;
			childTransform->SetDirty(true);
		}
		
	}
}
