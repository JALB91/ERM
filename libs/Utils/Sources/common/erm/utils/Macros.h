#pragma once

#define ERM_EXPAND(x) x

#if defined(ERM_WINDOWS)
#	define ERM_FUNC_SIG __FUNCSIG__
#elif defined(ERM_MAC)
#	define ERM_FUNC_SIG __PRETTY_FUNCTION__
#endif

#define ERM_GET_FIRST_ARG(arg1, ...)														arg1
#define ERM_GET_SECOND_ARG(arg1, arg2, ...)													arg2
#define ERM_GET_THIRD_ARG(arg1, arg2, arg3, ...)											arg3
#define ERM_GET_FOURTH_ARG(arg1, arg2, arg3, arg4, ...)										arg4
#define ERM_GET_FIFTH_ARG(arg1, arg2, arg3, arg4, arg5, ...)								arg5
#define ERM_GET_SIXTH_ARG(arg1, arg2, arg3, arg4, arg5, arg6, ...)							arg6
#define ERM_GET_SEVENTH_ARG(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...)					arg7
#define ERM_GET_EIGHTH_ARG(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ...)				arg8
#define ERM_GET_NINETH_ARG(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ...)		arg9
#define ERM_GET_TENTH_ARG(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, ...) arg10

#define ERM_EVAL0(...) __VA_ARGS__
#define ERM_EVAL1(...) ERM_EVAL0(ERM_EVAL0(ERM_EVAL0(__VA_ARGS__)))
#define ERM_EVAL2(...) ERM_EVAL1(ERM_EVAL1(ERM_EVAL1(__VA_ARGS__)))
#define ERM_EVAL3(...) ERM_EVAL2(ERM_EVAL2(ERM_EVAL2(__VA_ARGS__)))
#define ERM_EVAL4(...) ERM_EVAL3(ERM_EVAL3(ERM_EVAL3(__VA_ARGS__)))
#define ERM_EVAL(...)  ERM_EVAL4(ERM_EVAL4(ERM_EVAL4(__VA_ARGS__)))

#define ERM_MAP_END(...)
#define ERM_MAP_OUT
#define ERM_MAP_COMMA ,

#define ERM_MAP_GET_END2()			   0, ERM_MAP_END
#define ERM_MAP_GET_END1(...)		   ERM_MAP_GET_END2
#define ERM_MAP_GET_END(...)		   ERM_MAP_GET_END1
#define ERM_MAP_NEXT0(test, next, ...) next ERM_MAP_OUT
#define ERM_MAP_NEXT1(test, next)	   ERM_MAP_NEXT0(test, next, 0)
#define ERM_MAP_NEXT(test, next)	   ERM_MAP_NEXT1(ERM_MAP_GET_END test, next)

#define ERM_MAP0(f, x, peek, ...) f(x) ERM_MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define ERM_MAP1(f, x, peek, ...) f(x) ERM_MAP_NEXT(peek, ERM_MAP0)(f, peek, __VA_ARGS__)

#define ERM_MAP_LIST_NEXT1(test, next) ERM_MAP_NEXT0(test, ERM_MAP_COMMA next, 0)
#define ERM_MAP_LIST_NEXT(test, next)  ERM_MAP_LIST_NEXT1(ERM_MAP_GET_END test, next)

#define ERM_MAP_LIST0(f, x, peek, ...) f(x) ERM_MAP_LIST_NEXT(peek, ERM_MAP_LIST1)(f, peek, __VA_ARGS__)
#define ERM_MAP_LIST1(f, x, peek, ...) f(x) ERM_MAP_LIST_NEXT(peek, ERM_MAP_LIST0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro `f` to each of the remaining parameters.
 */
#define ERM_MAP(f, ...) ERM_EVAL(ERM_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/**
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 */
#define ERM_MAP_LIST(f, ...) ERM_EVAL(ERM_MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define ERM_ENUM_WITH_FLAGS(NAME, ...)                            \
	enum class NAME : uint32_t                                    \
	{                                                             \
		__VA_ARGS__,                                              \
		COUNT                                                     \
	};                                                            \
                                                                  \
	using NAME##Flags = Flags<NAME>;                              \
                                                                  \
	inline NAME##Flags                                            \
		operator|(NAME bit0, NAME bit1)                           \
	{                                                             \
		return NAME##Flags(bit0) | bit1;                          \
	}                                                             \
                                                                  \
	inline NAME##Flags                                            \
		operator&(NAME bit0, NAME bit1)                           \
	{                                                             \
		return NAME##Flags(bit0) & bit1;                          \
	}                                                             \
                                                                  \
	inline NAME##Flags                                            \
		operator^(NAME bit0, NAME bit1)                           \
	{                                                             \
		return NAME##Flags(bit0) ^ bit1;                          \
	}                                                             \
                                                                  \
	inline NAME##Flags operator~(NAME bits)                       \
                                                                  \
	{                                                             \
		return ~(NAME##Flags(bits));                              \
	}                                                             \
	inline NAME##Flags operator|(NAME bit0, uint32_t bit1)        \
	{                                                             \
		return NAME##Flags(bit0) | static_cast<NAME>(bit1);       \
	}                                                             \
	inline NAME##Flags                                            \
		operator&(NAME bit0, uint32_t bit1)                       \
	{                                                             \
		return NAME##Flags(bit0) & static_cast<NAME>(bit1);       \
	}                                                             \
                                                                  \
	inline NAME##Flags                                            \
		operator^(NAME bit0, uint32_t bit1)                       \
	{                                                             \
		return NAME##Flags(bit0) ^ static_cast<NAME>(bit1);       \
	}                                                             \
	inline NAME##Flags operator|(NAME##Flags bit0, uint32_t bit1) \
	{                                                             \
		return bit0 | static_cast<NAME>(bit1);                    \
	}                                                             \
	inline NAME##Flags                                            \
		operator&(NAME##Flags bit0, uint32_t bit1)                \
	{                                                             \
		return bit0 & static_cast<NAME>(bit1);                    \
	}                                                             \
                                                                  \
	inline NAME##Flags                                            \
		operator^(NAME##Flags bit0, uint32_t bit1)                \
	{                                                             \
		return bit0 ^ static_cast<NAME>(bit1);                    \
	}                                                             \
	inline bool                                                   \
		operator==(NAME bit0, uint32_t bit1)                      \
	{                                                             \
		return bit0 == static_cast<NAME>(bit1);                   \
	}

#define ERM_SENSIBLE_MEMBER_3(NAME, TYPE, DEFAULT) ERM_SENSIBLE_MEMBER_4(NAME, TYPE, DEFAULT, UpdateDirtyMode::NORMAL)

#define ERM_SENSIBLE_MEMBER_2(NAME, TYPE) ERM_SENSIBLE_MEMBER_3(NAME, TYPE, {})

#define ERM_SENSIBLE_MEMBER_MACRO_CHOOSER(...) \
	ERM_EXPAND(ERM_GET_FIFTH_ARG(__VA_ARGS__, ERM_SENSIBLE_MEMBER_4, ERM_SENSIBLE_MEMBER_3, ERM_SENSIBLE_MEMBER_2, ))

#define ERM_SENSIBLE_MEMBER(...) ERM_EXPAND(ERM_SENSIBLE_MEMBER_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#define ERM_DECLARE_LIB_BEGIN(NAME) \
class NAME##Lib final               \
{                                      \
public:                                \
	void Init();                       \
	void Deinit();                     \
							           \
	void OnPreUpdate();                \
	void OnUpdate(float dt);           \
	void OnPostUpdate();               \
								       \
	void OnPreRender();                \
	void OnRender();                   \
	void OnPostRender();

#define ERM_STACK_LIB_OBJECT(NAME)                  \
public:                                                \
	NAME m##NAME;                                      \
													   \
	NAME& Get##NAME() { return m##NAME; }              \
	const NAME& Get##NAME() const { return m##NAME; }

#define ERM_UNIQUE_LIB_OBJECT(NAME)                   \
public:                                                  \
	std::unique_ptr<NAME> m##NAME;                       \
														 \
	NAME& Get##NAME() { return *m##NAME; }               \
	const NAME& Get##NAME() const { return *m##NAME; }

#define ERM_SHARED_LIB_OBJECT(NAME)                   \
public:                                                  \
	std::shared_ptr<NAME> m##NAME;                       \
                                                         \
	NAME& Get##NAME() { return *m##NAME; }               \
	const NAME& Get##NAME() const { return *m##NAME; }

#define ERM_DECLARE_LIB_END(NAME) \
};                                   \
                                     \
extern NAME##Lib g##NAME##Lib;

