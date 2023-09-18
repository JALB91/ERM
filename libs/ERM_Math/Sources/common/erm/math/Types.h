#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>

namespace erm {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

template<u8 T, typename S>
using vec = glm::vec<T, S>;
using vec2 = vec<2, float>;
using vec3 = vec<3, float>;
using vec4 = vec<4, float>;
using ivec2 = vec<2, i32>;
using ivec3 = vec<3, i32>;
using ivec4 = vec<4, i32>;
using uvec2 = vec<2, u32>;
using uvec3 = vec<3, u32>;
using uvec4 = vec<4, u32>;

template<u8 R, u8 C, typename S>
using mat = glm::mat<R, C, S>;
using mat2 = mat<2, 2, float>;
using mat3 = mat<3, 3, float>;
using mat4 = mat<4, 4, float>;

using quat = glm::quat;

}