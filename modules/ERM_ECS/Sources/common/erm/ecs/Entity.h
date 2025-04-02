#pragma once

#include "erm/ecs/ECS.h"
#include "erm/ecs/ECSConfig.h"

#include <erm/utils/StaticString.h>

#include <string_view>
#include <utility>

namespace erm::ecs {

struct Entity
{
public:
	friend class ECS;

public:
	~Entity()
	{
		mECS.onEntityBeingRemoved(mId);
	}

	Entity(Entity&&) = delete;
	Entity(const Entity&) = delete;

	Entity& operator=(Entity&&) = delete;
	Entity& operator=(const Entity&) = delete;

	bool operator==(const Entity& other) const
	{
		return other.mId == mId;
	}

	inline EntityId getId() const { return mId; }
	inline bool isValid() const { return mId.isValid(); }

	inline ECS& getECS() const { return mECS; }

	inline const str128& getName() const { return mName; }
	inline void setName(std::string_view name) { mName = name; }

	template<typename T>
	inline bool hasComponent() const
	{
		return mECS.getSystem<typename T::SYSTEM_TYPE>()->hasComponent(mId);
	}

	template<typename T>
	inline T* getComponent() const
	{
		return mECS.getSystem<typename T::SYSTEM_TYPE>()->getComponent(mId);
	}

	template<typename T, typename... Args>
	inline T* addComponent(Args&&... args) const
	{
		return mECS.getSystem<typename T::SYSTEM_TYPE>()->addComponent(mId, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	inline T* requireComponent(Args&&... args) const
	{
		return mECS.getSystem<typename T::SYSTEM_TYPE>()->requireComponent(mId, std::forward<Args>(args)...);
	}

	inline EntityId getParent() const { return mParent; }
	inline const std::vector<EntityId>& getChildren() const { return mChildren; }

	inline void addChild(EntityId childId) { mECS.addChildToEntity(mId, childId); }
	inline void addChild(Entity& child) { mECS.addChildToEntity(mId, child.getId()); }

private:
	inline Entity(ECS& ecs, EntityId id, std::string_view name)
		: mECS(ecs)
		, mId(id)
		, mName(name)
	{}

	ECS& mECS;
	const EntityId mId;
	str128 mName;
	EntityId mParent;
	std::vector<EntityId> mChildren;
};

} // namespace erm::ecs
