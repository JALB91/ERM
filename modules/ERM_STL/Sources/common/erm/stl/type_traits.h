#pragma once

#include <optional>

namespace erm::stl {

template<typename T>
struct remove_all
{
	using type = std::remove_pointer<std::remove_reference_t<std::remove_cv_t<T>>>;
};

template<typename T>
using remove_all_t = typename remove_all<T>::type;

namespace internal {

template<typename T, typename Enable = void>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::optional<T>> : std::true_type {};

}

template<typename T>
constexpr bool is_optional_v = internal::is_optional<T>::value;

}