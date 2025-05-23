#include "erm/ecs/Entity.h"

#include "erm/ecs/ECS.h"

namespace erm::ecs {

Entity::~Entity()
{
	mECS.onEntityBeingRemoved(mId);
}

void Entity::addChild(EntityId childId)
{
	mECS.addChildToEntity(mId, childId);
}

void Entity::addChild(Entity& child)
{
	mECS.addChildToEntity(mId, child.getId());
}

}
