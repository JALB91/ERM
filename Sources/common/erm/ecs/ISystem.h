#pragma once

#include "erm/ecs/ECSConfig.h"
#include "erm/ecs/EntityId.h"

#include "erm/engine/Engine.h"

#include <array>
#include <memory>
#include <type_traits>
#include <utility>

namespace erm::ecs {
class ECS;
} // namespace erm::ecs

#define ERM_SYSTEM_DECL(NAME)                                                                         \
public:                                                                                               \
	friend class ECS;                                                                                 \
                                                                                                      \
	static const ID SYSTEM_ID;                                                                        \
                                                                                                      \
public:                                                                                               \
	void RemoveComponent(EntityId id) override;                                                       \
	const NAME##Component* GetComponent(EntityId id) const;                                           \
	NAME##Component* GetComponent(EntityId id);                                                       \
                                                                                                      \
	template<typename... Args>                                                                        \
	inline NAME##Component* AddComponent(EntityId id, Args&&... args)                                 \
	{                                                                                                 \
		if (!HasComponent(id))                                                                        \
		{                                                                                             \
			OnComponentBeingAdded(id);                                                                \
			mComponentsBitmask[id()] = true;                                                          \
			mComponents[id()] = NAME##Component(std::forward<Args>(args)...);                         \
			OnComponentAdded(id);                                                                     \
		}                                                                                             \
                                                                                                      \
		return GetComponent(id);                                                                      \
	}                                                                                                 \
                                                                                                      \
	template<typename... Args>                                                                        \
	inline NAME##Component* RequireComponent(EntityId id, Args&&... args)                             \
	{                                                                                                 \
		return (HasComponent(id) ? GetComponent(id) : AddComponent(id, std::forward<Args>(args)...)); \
	}                                                                                                 \
                                                                                                      \
	template<typename F>                                                                              \
	inline void ForEachComponent(F f)                                                                 \
	{                                                                                                 \
		for (ID i = 0; i < MAX_ID; ++i)                                                               \
		{                                                                                             \
			if (NAME##Component* c = GetComponent(i))                                                 \
			{                                                                                         \
				f(*c, i);                                                                             \
			}                                                                                         \
		}                                                                                             \
	}                                                                                                 \
                                                                                                      \
	template<typename F>                                                                              \
	inline void ForEachComponent(F f) const                                                           \
	{                                                                                                 \
		for (ID i = 0; i < MAX_ID; ++i)                                                               \
		{                                                                                             \
			if (const NAME##Component* c = GetComponent(i))                                           \
			{                                                                                         \
				f(*c, i);                                                                             \
			}                                                                                         \
		}                                                                                             \
	}                                                                                                 \
                                                                                                      \
private:                                                                                              \
	std::array<NAME##Component, MAX_ID> mComponents = {};

#define ERM_SYSTEM_IMPL(NAME)                                            \
	const ID NAME##System::SYSTEM_ID = NAME##SystemId;                   \
                                                                         \
	const NAME##Component* NAME##System::GetComponent(EntityId id) const \
	{                                                                    \
		return (HasComponent(id) ? &mComponents[id()] : nullptr);        \
	}                                                                    \
                                                                         \
	NAME##Component* NAME##System::GetComponent(EntityId id)             \
	{                                                                    \
		return (HasComponent(id) ? &mComponents[id()] : nullptr);        \
	}                                                                    \
                                                                         \
	void NAME##System::RemoveComponent(EntityId id)                      \
	{                                                                    \
		if (!HasComponent(id))                                           \
		{                                                                \
			return;                                                      \
		}                                                                \
		OnComponentBeingRemoved(id);                                     \
		mComponentsBitmask[id()] = false;                                \
		mComponents[id()] = {};                                          \
		OnComponentRemoved(id);                                          \
	}

namespace erm::ecs {

class ISystem
{
public:
	ISystem(Engine& engine)
		: mEngine(engine)
		, mECS(mEngine.GetECS())
		, mComponentsBitmask {false}
	{}
	virtual ~ISystem() = default;

	virtual void Init() = 0;

	virtual void OnPreUpdate() {}
	virtual void OnUpdate(float /*dt*/) {}
	virtual void OnPostUpdate() {}
	virtual void OnPreRender() {}
	virtual void OnRender() {}
	virtual void OnPostRender() {}

	inline virtual bool HasComponent(EntityId id) const final
	{
		return (id.IsValid() && mComponentsBitmask[id()]);
	}

	template<typename T>
	inline const T* GetComponent(EntityId id) const
	{
		return static_cast<const typename T::SYSTEM_TYPE*>(this)->GetComponent(id);
	}

	template<typename T>
	inline T* GetComponent(EntityId id)
	{
		return static_cast<typename T::SYSTEM_TYPE*>(this)->GetComponent(id);
	}

	template<typename T, typename... Args>
	inline T* AddComponent(EntityId id, Args&&... args)
	{
		return static_cast<typename T::SYSTEM_TYPE*>(this)->AddComponent(id, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	inline T* RequireComponent(EntityId id, Args&&... args)
	{
		return static_cast<typename T::SYSTEM_TYPE*>(this)->RequireComponent(id, std::forward<Args>(args)...);
	}

	virtual void RemoveComponent(EntityId id) = 0;

protected:
	inline virtual void OnComponentBeingAdded(EntityId /*id*/) {}
	inline virtual void OnComponentAdded(EntityId /*id*/) {}

	inline virtual void OnComponentBeingRemoved(EntityId /*id*/) {}
	inline virtual void OnComponentRemoved(EntityId /*id*/) {}

	Engine& mEngine;
	ECS& mECS;
	std::array<bool, MAX_ID> mComponentsBitmask;
};

} // namespace erm::ecs
