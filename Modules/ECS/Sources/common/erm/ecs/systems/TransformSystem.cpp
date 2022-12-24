#include "erm/ecs/systems/TransformSystem.h"

#include "erm/ecs/ECSUtils.h"
#include "erm/ecs/Entity.h"

#include "erm/utils/Profiler.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm::ecs {

ERM_SYSTEM_IMPL(Transform)

TransformSystem::TransformSystem(ECS& ecs)
	: ISystem(ecs)
{}

void TransformSystem::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

	UpdateDirtyRecursive(ROOT_ID);
}

void TransformSystem::OnEntityParentChanged(EntityId entityId)
{
	if (TransformComponent* component = GetComponent(entityId))
		component->SetDirty(true, UpdateDirtyMode::RECURSIVE);
}

void TransformSystem::UpdateDirtyRecursive(EntityId id)
{
	TransformComponent* transform = GetComponent(id);
	Entity* entity = mECS.GetEntityById(id);

	if (!transform || !entity)
		return;

	if (transform->IsDirty())
	{
		transform->mWorldTransform = glm::identity<math::mat4>();
		transform->mLocalTransform = glm::identity<math::mat4>();

		EntityId parent = entity->GetParent();

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

	for (const auto& childId : entity->GetChildren())
	{
		UpdateDirtyRecursive(childId);
	}
}

} // namespace erm::ecs
