#include "erm/ecs/ECS.h"

#include "erm/ecs/systems/AnimationSystem.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/RenderingSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include <erm/utils/Profiler.h>

namespace erm::ecs {

ECS::ECS() noexcept
{
	addSystem<TransformSystem>();
	addSystem<CameraSystem>();
	addSystem<RenderingSystem>();
	addSystem<SkeletonSystem>();
	addSystem<AnimationSystem>();
	addSystem<LightSystem>();
	addSystem<ModelSystem>();

	forEachSystem([](auto& system) {
		system.init();
	});

	mEntities[ROOT_ID].emplace(Entity(*this, ROOT_ID, "Root"));
	getSystem<TransformSystem>()->ISystem::addComponent(ROOT_ID);
}

ECS::~ECS() = default;

void ECS::preUpdate()
{
	ERM_PROFILE_FUNCTION();

	forEachSystem([](auto& system) {
		system.preUpdate();
	});
}

void ECS::update(float dt)
{
	ERM_PROFILE_FUNCTION();

	forEachSystem([dt](auto& system) {
		system.update(dt);
	});
}

void ECS::postUpdate()
{
	ERM_PROFILE_FUNCTION();

	forEachSystem([](auto& system) {
		system.postUpdate();
	});
}

void ECS::preRender()
{
	ERM_PROFILE_FUNCTION();

	forEachSystem([](auto& system) {
		system.preRender();
	});
}

void ECS::render()
{
	ERM_PROFILE_FUNCTION();

	forEachSystem([](auto& system) {
		system.render();
	});
}

void ECS::postRender()
{
	ERM_PROFILE_FUNCTION();

	forEachSystem([](auto& system) {
		system.postRender();
	});
}

void ECS::removeEntity(EntityId id)
{
	auto* entity = getEntityById(id);
	if (entity != nullptr)
	{
		for (EntityId child : entity->getChildren())
		{
			removeEntity(child);
		}

		auto* parent = getEntityById(entity->getParent());
		if (parent != nullptr)
		{
			parent->mChildren.erase(std::find(parent->mChildren.begin(), parent->mChildren.end(), id));
		}

		mEntities[id()].reset();
	}
}

void ECS::onEntityBeingRemoved(EntityId id)
{
	forEachSystem([id](auto& system) {
		system.removeComponent(id);
	});
}

Entity* ECS::getRoot()
{
	return getEntityById(ROOT_ID);
}

Entity* ECS::getOrCreateEntity(std::string_view name /*= "Unknown"*/)
{
	for (ID i = ROOT_ID; i < MAX_ID; ++i)
	{
		if (!mEntities[i] || !mEntities[i]->isValid())
		{
			auto& entity = mEntities[i].emplace(Entity(*this, i, name));
			getSystem<TransformSystem>()->addComponent(i);
			return &entity;
		}
	}

	return nullptr;
}

Entity* ECS::getEntityById(EntityId id)
{
	return ((id.isValid() && mEntities[id()]) ? &mEntities[id()].value() : nullptr);
}

void ECS::addChildToEntity(EntityId parentId, EntityId childId)
{
	if (!parentId.isValid() || !childId.isValid() || parentId == childId || childId == ROOT_ID)
	{
		return;
	}

	auto* child = getEntityById(childId);
	auto* parent = getEntityById(parentId);
	if (!parent || !child || child->getParent() == parentId)
	{
		return;
	}

	if (auto* previousParent = getEntityById(child->getParent()))
	{
		previousParent->mChildren.erase(std::find(previousParent->mChildren.begin(), previousParent->mChildren.end(), childId));
	}

	parent->mChildren.emplace_back(childId);
	child->mParent = parentId;

	forEachSystem([childId](auto& system) {
		system.onEntityParentChanged(childId);
	});
}

template<typename T>
void ECS::forEachSystem(const T& function)
{
	for (auto& system : mSystems)
	{
		if (system)
		{
			function(*system);
		}
	}
}

} // namespace erm::ecs
