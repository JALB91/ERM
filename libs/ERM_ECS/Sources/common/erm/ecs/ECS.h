#pragma once

#include "erm/ecs/ECSConfig.h"
#include "erm/ecs/EntityId.h"

#include <erm/utils/assert/Assert.h>
#include <erm/utils/Utils.h>

#include <array>
#include <memory>
#include <string_view>
#include <vector>

namespace erm::ecs {
struct Entity;
class ISystem;
} // namespace erm::ecs

namespace erm::ecs {

class ECS
{
public:
	ECS();
	~ECS();
	
	void init();

	void preUpdate();
	void update(float dt);
	void postUpdate();
	void preRender();
	void render();
	void postRender();

	template<typename T>
	T& addSystem()
	{
		if (ERM_EXPECT(mSystems.size() <= T::SYSTEM_ID || mSystems[T::SYSTEM_ID] == nullptr, "Trying to add a system twice"))
		{
			mSystems.resize(std::max(static_cast<int>(mSystems.size()), static_cast<int>(T::SYSTEM_ID + 1)));
			mSystems[T::SYSTEM_ID] = std::make_unique<T>(*this);
		}

		return static_cast<T&>(*mSystems[T::SYSTEM_ID]);
	}

	template<typename T>
	T* getSystem()
	{
		return static_cast<T*>(mSystems[T::SYSTEM_ID].get());
	}

	template<typename T>
	T* getComponent(EntityId id)
	{
		auto* system = getSystem<typename T::SYSTEM_TYPE>();
		return system ? system->getComponent(id) : nullptr;
	}

	void removeEntity(EntityId id);
	void onEntityBeingRemoved(EntityId id);
	void addChildToEntity(EntityId parentId, EntityId childId);

	Entity* getRoot();
	Entity* getOrCreateEntity(std::string_view name = "Unknown");
	Entity* getEntityById(EntityId id);

private:
	template<typename T>
	void forEachSystem(const T& function);

	std::vector<std::unique_ptr<ISystem>> mSystems;
	std::array<std::unique_ptr<Entity>, MAX_ID> mEntities;
};

} // namespace erm::ecs
