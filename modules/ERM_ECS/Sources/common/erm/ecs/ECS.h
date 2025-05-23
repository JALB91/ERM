#pragma once

#include "erm/ecs/ECSConfig.h"
#include "erm/ecs/Entity.h"
#include "erm/ecs/EntityId.h"
#include "erm/ecs/ISystem.h"

#include <erm/log/Assert.h>
#include <erm/modules_lib/IModuleObject.h>
#include <erm/utils/Utils.h>

#include <refl.hpp>

#include <array>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace erm::ecs {
struct Entity;
} // namespace erm::ecs

namespace erm::ecs {

class ECS : public IModuleObject<ECS>
{
public:
	ECS() noexcept;
	~ECS() override;

	void preUpdate();
	void update(float dt);
	void postUpdate();
	void preRender();
	void render();
	void postRender();

	template<typename T, typename ...Args>
	T& addSystem(Args... args)
	{
		if (ERM_EXPECT(mSystems.size() <= T::SYSTEM_ID || mSystems[T::SYSTEM_ID] == nullptr, "Trying to add a system twice"))
		{
			mSystems.resize(std::max(static_cast<int>(mSystems.size()), static_cast<int>(T::SYSTEM_ID + 1)));
			mSystems[T::SYSTEM_ID] = std::make_unique<T>(*this, std::forward<Args>(args)...);
		}

		return static_cast<T&>(*mSystems[T::SYSTEM_ID]);
	}

	template<typename T>
	T* getSystem()
	{
		return static_cast<T*>(mSystems[T::SYSTEM_ID].get());
	}
	
	template<typename T>
	bool hasComponent(EntityId id)
	{
		return getSystem<typename T::SYSTEM_TYPE>()->hasComponent(id);
	}

	template<typename T>
	T* getComponent(EntityId id)
	{
		return getSystem<typename T::SYSTEM_TYPE>()->getComponent(id);
	}
	
	template<typename T, typename... Args>
	T* addComponent(EntityId id, Args&&... args)
	{
		return getSystem<typename T::SYSTEM_TYPE>()->addComponent(id, std::forward<Args>(args)...);
	}
	
	template<typename T, typename... Args>
	T* requireComponent(EntityId id, Args&&... args)
	{
		return getSystem<typename T::SYSTEM_TYPE>()->requireComponent(id, std::forward<Args>(args)...);
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

	std::array<std::optional<Entity>, MAX_ID> mEntities;
	std::vector<std::unique_ptr<ISystem>> mSystems;
	
};

} // namespace erm::ecs

REFL_AUTO(
	type(erm::ecs::ECS)
);
