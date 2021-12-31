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
{}

ECS::~ECS()
{}

void ECS::Init()
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
}

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
			return mEntities[i].get();
		}
	}

	return nullptr;
}

Entity* ECS::GetEntityById(EntityId id)
{
	return ((id.IsValid() && mEntities[id()]) ? mEntities[id()].get() : nullptr);
}

template<typename T>
void ECS::ForEachSystem(const T& function)
{
	for (auto& system : mSystems)
		if (system)
			function(*system);
}

} // namespace erm::ecs
