#include "erm/ecs/systems/TransformSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/Entity.h"

#include "erm/utils/Profiler.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm::ecs {

ERM_SYSTEM_IMPL(Transform)

TransformSystem::TransformSystem(Engine& engine)
	: ISystem(engine)
{}

void TransformSystem::OnComponentAdded(EntityId id)
{
	if (id != ROOT_ID && GetComponent(id) && !GetComponent(id)->mParent.IsValid())
		AddChild(ROOT_ID, id);
}

void TransformSystem::OnComponentBeingRemoved(EntityId id)
{
	TransformComponent* transform = GetComponent(id);

	ERM_ASSERT(transform);

	for (EntityId entityId : transform->mChildren)
	{
		TransformComponent* entityTransform = GetComponent(entityId);
		entityTransform->mParent.Reset();
		entityTransform->SetDirty(true);
	}
	transform->mChildren.clear();

	RemoveFromParent(id);
}

void TransformSystem::UpdateDirtyRecursive(EntityId id)
{
	TransformComponent* transform = GetComponent(id);

	ERM_ASSERT(transform);

	if (!transform)
		return;

	if (transform->IsDirty())
	{
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

	for (const auto& entityId : transform->mChildren)
	{
		UpdateDirtyRecursive(entityId);
	}
}

void TransformSystem::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

	UpdateDirtyRecursive(ROOT_ID);
}

void TransformSystem::RemoveFromParent(EntityId id)
{
	if (!id.IsValid())
		return;

	TransformComponent* entityTransform = GetComponent(id);
	EntityId parentId = entityTransform->mParent;

	if (!parentId.IsValid())
		return;

	std::vector<EntityId>& children = GetComponent(parentId)->mChildren;
	children.erase(std::find(children.begin(), children.end(), id));

	entityTransform->mParent.Reset();
	entityTransform->SetDirty(true);
}

void TransformSystem::AddChild(EntityId parent, EntityId child)
{
	if (!parent.IsValid() || !child.IsValid() || parent == child || child() == ROOT_ID)
		return;

	TransformComponent* parentTransform = GetComponent(parent);
	TransformComponent* childTransform = GetComponent(child);

	if (childTransform->mParent == parent)
		return;

	RemoveFromParent(child);
	parentTransform->mChildren.emplace_back(child);
	childTransform->mParent = parent;
	childTransform->SetDirty(true);
}

} // namespace erm::ecs
