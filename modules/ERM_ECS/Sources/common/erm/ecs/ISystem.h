#pragma once

#include "erm/ecs/ECSConfig.h"
#include "erm/ecs/EntityId.h"

#include <erm/log/Assert.h>

#include <erm/utils/Utils.h>

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
	static constexpr ID SYSTEM_ID = NAME##SystemId;                                                   \
                                                                                                      \
public:                                                                                               \
	void removeComponent(EntityId id) override;                                                       \
	const NAME##Component* getComponent(EntityId id) const;                                           \
	NAME##Component* getComponent(EntityId id);                                                       \
                                                                                                      \
	template<typename... Args>                                                                        \
	inline NAME##Component* addComponent(EntityId id, Args&&... args)                                 \
	{                                                                                                 \
		ERM_ASSERT(id.isValid());                                                                     \
		if (!hasComponent(id))                                                                        \
		{                                                                                             \
			onComponentBeingAdded(id);                                                                \
			ISystem::addComponent(id);                                                                \
			mComponents[id()] = NAME##Component(std::forward<Args>(args)...);                         \
			mComponents[id()].mId = id;                                                               \
			onComponentAdded(id);                                                                     \
		}                                                                                             \
                                                                                                      \
		return getComponent(id);                                                                      \
	}                                                                                                 \
                                                                                                      \
	template<typename... Args>                                                                        \
	inline NAME##Component* requireComponent(EntityId id, Args&&... args)                             \
	{                                                                                                 \
		ERM_ASSERT(id.isValid());                                                                     \
		return (hasComponent(id) ? getComponent(id) : addComponent(id, std::forward<Args>(args)...)); \
	}                                                                                                 \
                                                                                                      \
	template<typename F>                                                                              \
	inline void forEachComponent(F f)                                                                 \
	{                                                                                                 \
		for (ID i = 0; i < MAX_ID; ++i)                                                               \
		{                                                                                             \
			if (NAME##Component* c = getComponent(i))                                                 \
			{                                                                                         \
				f(*c);                                                                                \
			}                                                                                         \
		}                                                                                             \
	}                                                                                                 \
                                                                                                      \
	template<typename F>                                                                              \
	inline void forEachComponent(F f) const                                                           \
	{                                                                                                 \
		for (ID i = 0; i < MAX_ID; ++i)                                                               \
		{                                                                                             \
			if (const NAME##Component* c = getComponent(i))                                           \
			{                                                                                         \
				f(*c);                                                                                \
			}                                                                                         \
		}                                                                                             \
	}                                                                                                 \
                                                                                                      \
private:                                                                                              \
	std::array<NAME##Component, MAX_ID> mComponents = {};

#define ERM_SYSTEM_IMPL(NAME)                                            \
	const NAME##Component* NAME##System::getComponent(EntityId id) const \
	{                                                                    \
		return (hasComponent(id) ? &mComponents[id()] : nullptr);        \
	}                                                                    \
                                                                         \
	NAME##Component* NAME##System::getComponent(EntityId id)             \
	{                                                                    \
		return (hasComponent(id) ? &mComponents[id()] : nullptr);        \
	}                                                                    \
                                                                         \
	void NAME##System::removeComponent(EntityId id)                      \
	{                                                                    \
		if (!hasComponent(id))                                           \
		{                                                                \
			return;                                                      \
		}                                                                \
		onComponentBeingRemoved(id);                                     \
		ISystem::removeComponent(id);                                    \
		mComponents[id()] = {};                                          \
		onComponentRemoved(id);                                          \
	}

namespace erm::ecs {

class ISystem
{
public:
	friend class ECS;

public:
	ISystem(ECS& ecs)
		: mECS(ecs)
		, mComponentsBitmask {false}
	{}
	virtual ~ISystem() = default;

	ISystem(const ISystem&) = delete;
	ISystem(ISystem&&) = delete;

	ISystem& operator=(const ISystem&) = delete;
	ISystem& operator=(ISystem&&) = delete;

	inline virtual void removeComponent(EntityId id)
	{
		ERM_ASSERT(id.isValid());
		mComponentsBitmask[id()] = false;
	}

	inline virtual bool hasComponent(EntityId id) const final
	{
		return (id.isValid() && mComponentsBitmask[id()]);
	}

protected:
	inline virtual void init() {}
	inline virtual void preUpdate() {}
	inline virtual void update(float /*dt*/) {}
	inline virtual void postUpdate() {}
	inline virtual void preRender() {}
	inline virtual void render() {}
	inline virtual void postRender() {}
	inline virtual void onEntityParentChanged(EntityId /*entityId*/) {}

	inline virtual void onComponentBeingAdded(EntityId /*id*/) {}
	inline virtual void onComponentAdded(EntityId /*id*/) {}

	inline virtual void onComponentBeingRemoved(EntityId /*id*/) {}
	inline virtual void onComponentRemoved(EntityId /*id*/) {}

	inline virtual void addComponent(EntityId id) final
	{
		mComponentsBitmask[id()] = true;
	}

	ECS& mECS;

private:
	std::array<bool, MAX_ID> mComponentsBitmask;
};

} // namespace erm::ecs
