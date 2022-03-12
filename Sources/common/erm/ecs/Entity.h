#pragma once

#include "erm/ecs/ECS.h"
#include "erm/ecs/ECSConfig.h"

#include <string>
#include <utility>

namespace erm::ecs {

struct Entity
{
public:
	friend class ECS;

public:
	~Entity()
	{
		mECS.OnEntityBeingRemoved(mId);
	}

	Entity(Entity&&) = delete;
	Entity(const Entity&) = delete;

	Entity& operator=(Entity&&) = delete;
	Entity& operator=(const Entity&) = delete;

	bool operator==(const Entity& other) const
	{
		return other.mId == mId;
	}

	inline EntityId GetId() const { return mId; }
	inline bool IsValid() const { return mId.IsValid(); }

	inline ECS& GetECS() const { return mECS; }

	inline const std::string& GetName() const { return mName; }
	inline void SetName(const std::string& name) { mName = name; }

	template<typename T>
	inline bool HasComponent() const
	{
		return mECS.GetSystem<typename T::SYSTEM_TYPE>()->HasComponent(mId);
	}

	template<typename T>
	inline T* GetComponent() const
	{
		return mECS.GetSystem<typename T::SYSTEM_TYPE>()->GetComponent(mId);
	}

	template<typename T, typename... Args>
	inline T* AddComponent(Args&&... args) const
	{
		return mECS.GetSystem<typename T::SYSTEM_TYPE>()->AddComponent(mId, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	inline T* RequireComponent(Args&&... args) const
	{
		return mECS.GetSystem<typename T::SYSTEM_TYPE>()->RequireComponent(mId, std::forward<Args>(args)...);
	}

	inline EntityId GetParent() const { return mParent; }
	inline const std::vector<EntityId>& GetChildren() const { return mChildren; }

	inline void AddChild(EntityId childId) { mECS.AddChildToEntity(mId, childId); }
	inline void AddChild(Entity& child) { mECS.AddChildToEntity(mId, child.GetId()); }

private:
	inline Entity(ECS& ecs, EntityId id, const std::string& name)
		: mECS(ecs)
		, mId(id)
		, mName(name)
	{}

	ECS& mECS;
	const EntityId mId;
	std::string mName;
	EntityId mParent;
	std::vector<EntityId> mChildren;
};

} // namespace erm::ecs
