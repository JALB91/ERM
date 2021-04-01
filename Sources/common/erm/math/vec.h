#pragma once

#include <glm/glm.hpp>

namespace erm::math {

template<int T, typename S>
using vec = glm::vec<T, S>;

typedef vec<2, float> vec2;
typedef vec<3, float> vec3;
typedef vec<4, float> vec4;

} // namespace erm::math
