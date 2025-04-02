#pragma once

#include <type_traits>

namespace erm::concepts {

template<typename T>
concept is_enum = std::is_enum_v<T>;

template<typename T>
concept is_not_enum = !std::is_enum_v<T>;

}