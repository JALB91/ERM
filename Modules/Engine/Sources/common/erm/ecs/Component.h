#pragma once

#include "erm/ecs/ComponentId.h"
#include "erm/ecs/ECS.h"
#include "erm/ecs/Entity.h"
#include "erm/ecs/UpdateDirtyMode.h"

#include "erm/utils/Macros.h"

#include <type_traits>

namespace erm::ecs {
class ISystem;
}

namespace erm::ecs {

#define ERM_COMPONENT_DECL(NAME)                                                            \
public:                                                                                     \
	typedef NAME##System SYSTEM_TYPE;                                                       \
	friend class NAME##System;                                                              \
                                                                                            \
public:                                                                                     \
	inline const ComponentId& GetComponentId() const { return mId; }                        \
	inline bool IsDirty() const { return mIsDirty; }                                        \
	void SetDirty(bool isDirty, UpdateDirtyMode updateDirtyMode = UpdateDirtyMode::NORMAL); \
                                                                                            \
private:                                                                                    \
	ComponentId mId = {};                                                                   \
	bool mIsDirty = true;

#define ERM_COMPONENT_IMPL(NAME)                                                                                \
	void NAME##Component::SetDirty(bool isDirty, UpdateDirtyMode updateDirtyMode /*= UpdateDirtyMode::NORMAL*/) \
	{                                                                                                           \
		mIsDirty = isDirty;                                                                                     \
                                                                                                                \
		if (updateDirtyMode == UpdateDirtyMode::RECURSIVE)                                                      \
		{                                                                                                       \
			auto& ecs = gEngine->GetECS();                                                                      \
			auto* entity = ecs.GetEntityById(GetComponentId());                                                 \
                                                                                                                \
			for (auto& child : entity->GetChildren())                                                           \
			{                                                                                                   \
				auto component = ecs.GetSystem<NAME##System>()->GetComponent(child);                            \
				if (component)                                                                                  \
					component->SetDirty(isDirty, updateDirtyMode);                                              \
			}                                                                                                   \
		}                                                                                                       \
	}

#define ERM_SENSIBLE_MEMBER_4(NAME, TYPE, DEFAULT, UPDATE_DIRTY_MODE)                 \
private:                                                                              \
	TYPE m##NAME = DEFAULT;                                                           \
                                                                                      \
public:                                                                               \
	template<typename T = TYPE, typename E = std::enable_if_t<!std::is_const_v<T>>>   \
	inline void Set##NAME(T value)                                                    \
	{                                                                                 \
		if (value == m##NAME)                                                         \
			return;                                                                   \
		m##NAME = std::forward<T>(value);                                             \
		SetDirty(true, UPDATE_DIRTY_MODE);                                            \
	}                                                                                 \
                                                                                      \
	template<typename T = TYPE, typename E = std::enable_if_t<std::is_pointer_v<T>>>  \
	inline const T Get##NAME() const                                                  \
	{                                                                                 \
		return m##NAME;                                                               \
	}                                                                                 \
                                                                                      \
	template<typename T = TYPE, typename E = std::enable_if_t<!std::is_pointer_v<T>>> \
	inline const T& Get##NAME() const                                                 \
	{                                                                                 \
		return m##NAME;                                                               \
	}

#define ERM_SENSIBLE_MEMBER_3(NAME, TYPE, DEFAULT) ERM_SENSIBLE_MEMBER_4(NAME, TYPE, DEFAULT, UpdateDirtyMode::NORMAL)

#define ERM_SENSIBLE_MEMBER_2(NAME, TYPE) ERM_SENSIBLE_MEMBER_3(NAME, TYPE, {})

#define ERM_MACRO_CHOOSER(...) \
	ERM_EXPAND(ERM_GET_FIFTH_ARG(__VA_ARGS__, ERM_SENSIBLE_MEMBER_4, ERM_SENSIBLE_MEMBER_3, ERM_SENSIBLE_MEMBER_2, ))

#define ERM_SENSIBLE_MEMBER(...) ERM_EXPAND(ERM_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

} // namespace erm::ecs
