#pragma once

#include <erm/math/Types.h>

#include <assert.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#define ERM_UNUSED(x)	   (void)x;
#define ERM_ASSERT(x)	   assert(x)
#define ERM_ASSERT_HARD(x) {const bool _result = (x); assert(_result); if (!_result) abort();}
#define ERM_EXPECT(x, msg) erm::utils::logCall((x), msg, #x, __FILE__, __LINE__)

#define ERM_DECL_SET_GET_OPT_WITH_DEFAULT(NAME, TYPE, DEFAULT)          \
private:                                                                \
	std::optional<TYPE> m##NAME;                                        \
                                                                        \
public:                                                                 \
	inline TYPE get##NAME() const { return m##NAME.value_or(DEFAULT); } \
	inline void set##NAME(const TYPE& v) { m##NAME = v; }               \
	inline bool has##NAME##Value() const { return m##NAME.has_value(); }

namespace erm::utils {

extern bool logCall(bool cond, std::string_view msg, std::string_view function, std::string_view file, int line);
extern void writeToFile(std::string_view path, std::string_view data);
extern std::string readFromFile(std::string_view path);
extern std::string formatTime(u64 seconds) noexcept;

inline constexpr std::vector<std::string> splitString(std::string_view str, char ch) noexcept
{
	std::vector<std::string> res;
	std::string* curr = str.empty() ? nullptr : &res.emplace_back();

	for (char c : str)
	{
		if (c == ch)
		{
			curr = &res.emplace_back();
		}
		else
		{
			*curr += c;
		}
	}

	return res;
}

inline constexpr char charToLower(char c) noexcept
{
	return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

inline constexpr char charToUpper(char c) noexcept
{
	return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

inline constexpr bool compareNoCaseSensitive(std::string_view a, std::string_view b) noexcept
{
	if (a.size() != b.size())
	{
		return false;
	}

	for (u64 i = 0; i < a.size(); ++i)
	{
		if (charToLower(a[i]) != charToLower(b[i]))
		{
			return false;
		}
	}

	return true;
}

inline constexpr bool endsWith(std::string_view s, std::string_view c) noexcept
{
	if (c.size() > s.size())
	{
		return false;
	}

	for (i64 i = static_cast<i64>(c.size()); i > 0; --i)
	{
		if (s[s.size() - i] != c[c.size() - i])
		{
			return false;
		}
	}

	return true;
}

inline constexpr u64 c_str_size(const char* const str) noexcept
{
	u64 size = 0;
	while (str[size] != '\0')
	{
		++size;
	}
	return size;
}

inline constexpr std::string_view stripFunctionName(std::string_view fn) noexcept
{
	std::string_view result = fn;

	result = result.substr(result.find("erm::") + 5);
	result = result.substr(0, result.find("("));

	return result;
}

template<
	typename T,
	typename Enable = std::enable_if_t<std::is_copy_constructible_v<T> && !std::is_pointer_v<T>>>
T clone(const T& value)
{
	return T {value};
}

template<
	typename T,
	typename Enable = std::enable_if_t<std::is_copy_constructible_v<T> && !std::is_pointer_v<T>>>
std::unique_ptr<T> clone(const std::unique_ptr<T>& value)
{
	ERM_ASSERT(value);
	return std::make_unique<T>(*value.get());
}

template<typename T>
struct remove_all
{
	using type = std::remove_pointer<std::remove_reference_t<std::remove_cv_t<T>>>;
};

template<typename T>
using remove_all_t = typename remove_all<T>::type;

} // namespace erm::Utils
