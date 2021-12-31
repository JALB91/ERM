#pragma once

#include <type_traits>

namespace erm::ecs {

#define ERM_SENSIBLE_MEMBER(NAME, TYPE, DEFAULT)                                      \
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
		SetDirty(true);                                                               \
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

struct IComponent
{
public:
	IComponent()
		: mIsDirty(true)
	{}
	virtual ~IComponent() = default;

	inline virtual void SetDirty(bool isDirty) { mIsDirty = isDirty; }
	inline bool IsDirty() const { return mIsDirty; }

private:
	bool mIsDirty;
};

} // namespace erm::ecs
