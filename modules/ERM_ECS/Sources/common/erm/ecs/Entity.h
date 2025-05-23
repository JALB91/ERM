#pragma once

#include "erm/ecs/ECSConfig.h"
#include "erm/ecs/EntityId.h"

#include <erm/utils/StaticString.h>

#include <functional>
#include <string_view>

namespace erm::ecs {
class ECS;
}

namespace erm::ecs {

struct Entity
{
public:
	friend class ECS;

public:
	Entity(const Entity&) = delete;
	Entity(Entity&&) noexcept = default;
	
	Entity& operator=(const Entity&) = delete;
	Entity& operator=(Entity&&) = delete;
	
	~Entity();

	bool operator==(const Entity& other) const
	{
		return other.mId == mId;
	}

	inline EntityId getId() const { return mId; }
	inline bool isValid() const { return mId.isValid(); }

	inline ECS& getECS() const { return mECS; }

	inline const str128& getName() const { return mName; }
	inline void setName(std::string_view name) { mName = name; }

	inline EntityId getParent() const { return mParent; }
	inline const std::vector<EntityId>& getChildren() const { return mChildren; }

	void addChild(EntityId childId);
	void addChild(Entity& child);

private:
	inline Entity(ECS& ecs, EntityId id, std::string_view name) noexcept
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
