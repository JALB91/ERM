#pragma once

#include <erm/math/Types.h>

#include <charconv>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#define ERM_DECL_SET_GET_OPT_WITH_DEFAULT(NAME, TYPE, DEFAULT)          \
private:                                                                \
	std::optional<TYPE> m##NAME;                                        \
                                                                        \
public:                                                                 \
	inline TYPE get##NAME() const { return m##NAME.value_or(DEFAULT); } \
	inline void set##NAME(const TYPE& v) { m##NAME = v; }               \
	inline bool has##NAME##Value() const { return m##NAME.has_value(); }

namespace erm::utils {

std::string formatTime(u64 seconds) noexcept;

template<typename T>
inline std::optional<T> parseNumber(std::string_view data)
{
	T value;
	const auto result = std::from_chars(data.data(), data.data() + data.size(), value);
	return result.ec == std::errc() ? std::make_optional(value) : std::nullopt;
}

#if defined(ERM_HOST_OSX)
template<>
inline std::optional<float> parseNumber(std::string_view data)
{
	return atof(data.data());
}
#endif

inline constexpr std::vector<std::string> splitString(std::string_view str, char ch) noexcept
{
	if (str.empty())
	{
		return {};
	}

	std::vector<std::string> result;
	size_t currStartOffset = 0;
	size_t currCount = 0;

	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == ch)
		{
			result.emplace_back(&str[currStartOffset], currCount);
			currStartOffset = i + 1;
			currCount = 0;
		}
		else
		{
			++currCount;
		}
	}

	if (currCount > 0)
	{
		result.emplace_back(&str[currStartOffset], currCount);
	}

	return result;
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

	for (size_t i = 0; i < a.size(); ++i)
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

inline constexpr size_t c_str_size(const char* const str) noexcept
{
	size_t size = 0;
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
	result = result.substr(0, result.find('('));

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
	return value ? std::make_unique<T>(*value.get()) : nullptr;
}

namespace internal {

template<std::size_t N>
struct num { static const constexpr auto value = N; };

template <class F, std::size_t OFFSET, std::size_t... IS>
void for_(F func, std::index_sequence<IS...>)
{
	(func(num<IS + OFFSET>{}), ...);
}

}

template <std::size_t OFFSET, std::size_t N, typename F>
void for_constexpr(F func)
{
	internal::for_<F, OFFSET>(func, std::make_index_sequence<N>());
}

} // namespace erm::Utils
