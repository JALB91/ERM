#pragma once

#include <glm/glm.hpp>

namespace erm::math {

template<int T, typename S>
using vec = glm::vec<T, S>;

using vec2 = vec<2, float>;
using vec3 = vec<3, float>;
using vec4 = vec<4, float>;
using ivec2 = vec<2, int>;
using ivec3 = vec<3, int>;
using ivec4 = vec<4, int>;

} // namespace erm::math
