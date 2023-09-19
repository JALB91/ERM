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
#define ERM_ASSERT_HARD(x) assert(x); if (!x) abort();
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
extern std::vector<std::string> splitString(std::string_view str, char ch);
extern std::string readFromFile(std::string_view path);
extern void writeToFile(std::string_view path, std::string_view data);
extern bool compareNoCaseSensitive(std::string_view a, std::string_view b);
extern bool endsWith(std::string_view s, std::string_view c);
extern std::string formatTime(u64 seconds);

constexpr std::string_view stripFunctionName(std::string_view fn)
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
