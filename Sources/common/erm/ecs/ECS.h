#pragma once

#include "erm/ecs/ECSConfig.h"
#include "erm/ecs/EntityId.h"

#include "erm/utils/Utils.h"

#include <array>
#include <memory>
#include <vector>

namespace erm {
class Engine;
namespace ecs {
struct Entity;
class ISystem;
} // namespace ecs
} // namespace erm

namespace erm::ecs {

class ECS
{
public:
	ECS(Engine& engine);
	~ECS();

	void Init();

	void OnPreUpdate();
	void OnUpdate(float dt);
	void OnPostUpdate();
	void OnPreRender();
	void OnRender();
	void OnPostRender();

	template<typename T>
	T& AddSystem()
	{
		if (ERM_EXPECT(mSystems.size() <= T::SYSTEM_ID || mSystems[T::SYSTEM_ID] == nullptr, "Trying to add a system twice"))
		{
			mSystems.resize(std::max(static_cast<int>(mSystems.size()), static_cast<int>(T::SYSTEM_ID + 1)));
			mSystems[T::SYSTEM_ID] = std::make_unique<T>(mEngine);
		}

		return static_cast<T&>(*mSystems[T::SYSTEM_ID]);
	}

	template<typename T>
	T* GetSystem()
	{
		return static_cast<T*>(mSystems[T::SYSTEM_ID].get());
	}

	void RemoveEntity(EntityId id);
	void OnEntityBeingRemoved(EntityId id);

	Entity* GetRoot();
	Entity* GetOrCreateEntity(const char* name = "Unknown");
	Entity* GetEntityById(EntityId id);

private:
	template<typename T>
	void ForEachSystem(const T& function);

	Engine& mEngine;
	std::vector<std::unique_ptr<ISystem>> mSystems;
	std::array<std::unique_ptr<Entity>, MAX_ID> mEntities;
};

} // namespace erm::ecs
