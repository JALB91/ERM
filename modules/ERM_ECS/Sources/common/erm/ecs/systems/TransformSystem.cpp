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

void TransformSystem::postUpdate()
{
	ERM_PROFILE_FUNCTION();

	updateDirtyRecursive(ROOT_ID);
}

void TransformSystem::onEntityParentChanged(EntityId entityId)
{
	auto* component = getComponent(entityId);
	if (component != nullptr)
	{
		component->setDirty(true, UpdateDirtyMode::RECURSIVE);
	}
}

void TransformSystem::updateDirtyRecursive(EntityId id)
{
	auto* transform = getComponent(id);
	auto* entity = mECS.getEntityById(id);

	if (transform == nullptr || entity == nullptr)
	{
		return;
	}

	if (transform->isDirty())
	{
		transform->mWorldTransform = glm::identity<mat4>();
		transform->mLocalTransform = glm::identity<mat4>();

		EntityId parent = entity->getParent();

		if (parent.isValid())
		{
			auto* parentTransform = getComponent(parent);
			if (parentTransform != nullptr)
			{
				transform->mWorldTransform = parentTransform->mWorldTransform;
			}
		}

		transform->mLocalTransform = glm::translate(transform->mLocalTransform, transform->mTranslation);
		transform->mLocalTransform = glm::rotate(transform->mLocalTransform, transform->mRotation.z, vec3(0.0f, 0.0f, 1.0f));
		transform->mLocalTransform = glm::rotate(transform->mLocalTransform, transform->mRotation.y, vec3(0.0f, 1.0f, 0.0f));
		transform->mLocalTransform = glm::rotate(transform->mLocalTransform, transform->mRotation.x, vec3(1.0f, 0.0f, 0.0f));
		transform->mLocalTransform = glm::scale(transform->mLocalTransform, transform->mScale);

		transform->mWorldTransform *= transform->mLocalTransform;

		transform->setDirty(false);
	}

	for (const auto& childId : entity->getChildren())
	{
		updateDirtyRecursive(childId);
	}
}

} // namespace erm::ecs
