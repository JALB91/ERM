#pragma once

#include <assert.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#define ERM_UNUSED(x)	   (void)x;
#define ERM_ASSERT(x)	   assert(x)
#define ERM_ASSERT_HARD(x) assert(x); if (!x) abort();
#define ERM_EXPECT(x, msg) erm::utils::LogCall((x), msg, #x, __FILE__, __LINE__)

#define ERM_DECL_SET_GET_OPT_WITH_DEFAULT(NAME, TYPE, DEFAULT)          \
private:                                                                \
	std::optional<TYPE> m##NAME;                                        \
                                                                        \
public:                                                                 \
	inline TYPE Get##NAME() const { return m##NAME.value_or(DEFAULT); } \
	inline void Set##NAME(const TYPE& v) { m##NAME = v; }               \
	inline bool Has##NAME##Value() const { return m##NAME.has_value(); }

namespace erm::utils {

extern bool LogCall(bool cond, std::string_view msg, std::string_view function, std::string_view file, int line);
extern std::vector<std::string> SplitString(std::string_view str, char ch);
extern std::string ReadFromFile(std::string_view path);
extern void WriteToFile(std::string_view path, std::string_view data);
extern bool CompareNoCaseSensitive(std::string_view a, std::string_view b);
extern bool EndsWith(std::string_view s, std::string_view c);
extern std::string FormatTime(unsigned int seconds);

constexpr std::string_view StripFunctionName(std::string_view fn)
{
	std::string_view result = fn;

	result = result.substr(result.find("erm::") + 5);
	result = result.substr(0, result.find("("));

	return result;
}

template<
	typename T,
	typename Enable = std::enable_if_t<std::is_copy_constructible_v<T> && !std::is_pointer_v<T>>>
T Clone(const T& value)
{
	return T {value};
}

template<
	typename T,
	typename Enable = std::enable_if_t<std::is_copy_constructible_v<T> && !std::is_pointer_v<T>>>
std::unique_ptr<T> Clone(const std::unique_ptr<T>& value)
{
	ERM_ASSERT(value);
	return std::make_unique<T>(*value.get());
}

template<typename T>
struct RemoveAll
{
	using type = std::remove_pointer<std::remove_reference_t<std::remove_cv_t<T>>>;
};

template<typename T>
using RemoveAll_t = typename RemoveAll<T>::type;

} // namespace erm::Utils
