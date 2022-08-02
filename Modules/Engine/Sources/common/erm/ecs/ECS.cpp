#include "erm/ecs/ECS.h"

#include "erm/ecs/Entity.h"
#include "erm/ecs/systems/AnimationSystem.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/EditorSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/RenderingSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/utils/Profiler.h"

namespace erm::ecs {

ECS::ECS(Engine& engine)
	: mEngine(engine)
{
	AddSystem<TransformSystem>();
	AddSystem<CameraSystem>();
	AddSystem<RenderingSystem>();
	AddSystem<EditorSystem>();
	AddSystem<SkeletonSystem>();
	AddSystem<AnimationSystem>();
	AddSystem<LightSystem>();
	AddSystem<ModelSystem>();

	ForEachSystem([](auto& system) {
		system.Init();
	});

	mEntities[ROOT_ID].reset(new Entity(*this, ROOT_ID, "Root"));
	mEntities[ROOT_ID]->AddComponent<TransformComponent>();
}

ECS::~ECS() = default;

void ECS::OnPreUpdate()
{
	ERM_PROFILE_FUNCTION();

	ForEachSystem([](auto& system) {
		system.OnPreUpdate();
	});
}

void ECS::OnUpdate(float dt)
{
	ERM_PROFILE_FUNCTION();

	ForEachSystem([dt](auto& system) {
		system.OnUpdate(dt);
	});
}

void ECS::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

	ForEachSystem([](auto& system) {
		system.OnPostUpdate();
	});
}

void ECS::OnPreRender()
{
	ERM_PROFILE_FUNCTION();

	ForEachSystem([](auto& system) {
		system.OnPreRender();
	});
}

void ECS::OnRender()
{
	ERM_PROFILE_FUNCTION();

	ForEachSystem([](auto& system) {
		system.OnRender();
	});
}

void ECS::OnPostRender()
{
	ERM_PROFILE_FUNCTION();

	ForEachSystem([](auto& system) {
		system.OnPostRender();
	});
}

void ECS::RemoveEntity(EntityId id)
{
	if (Entity* entity = GetEntityById(id))
	{
		for (EntityId child : entity->GetChildren())
		{
			RemoveEntity(child);
		}

		if (Entity* parent = GetEntityById(entity->GetParent()))
		{
			parent->mChildren.erase(std::find(parent->mChildren.begin(), parent->mChildren.end(), id));
		}

		mEntities[id()].reset();
	}
}

void ECS::OnEntityBeingRemoved(EntityId id)
{
	ForEachSystem([id](auto& system) {
		system.RemoveComponent(id);
	});
}

Entity* ECS::GetRoot()
{
	return GetEntityById(ROOT_ID);
}

Entity* ECS::GetOrCreateEntity(const char* name /*= "Unknown"*/)
{
	for (ID i = ROOT_ID; i < MAX_ID; ++i)
	{
		if (!mEntities[i] || !mEntities[i]->IsValid())
		{
			mEntities[i].reset(new Entity(*this, i, name));
			mEntities[i]->AddComponent<TransformComponent>();
			return mEntities[i].get();
		}
	}

	return nullptr;
}

Entity* ECS::GetEntityById(EntityId id)
{
	return ((id.IsValid() && mEntities[id()]) ? mEntities[id()].get() : nullptr);
}

void ECS::AddChildToEntity(EntityId parentId, EntityId childId)
{
	if (!parentId.IsValid() || !childId.IsValid() || parentId == childId || childId == ROOT_ID)
		return;

	auto* child = GetEntityById(childId);
	auto* parent = GetEntityById(parentId);
	if (!parent || !child || child->GetParent() == parentId)
		return;

	if (auto* previousParent = GetEntityById(child->GetParent()))
		previousParent->mChildren.erase(std::find(previousParent->mChildren.begin(), previousParent->mChildren.end(), childId));

	parent->mChildren.emplace_back(childId);
	child->mParent = parentId;

	ForEachSystem([childId](auto& system) {
		system.OnEntityParentChanged(childId);
	});
}

template<typename T>
void ECS::ForEachSystem(const T& function)
{
	for (auto& system : mSystems)
		if (system)
			function(*system);
}

} // namespace erm::ecs
