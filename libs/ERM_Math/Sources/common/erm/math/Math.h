#pragma once

#include "erm/math/Types.h"

namespace erm::math {

extern void decomposeMatrix(
	const mat4& matrix,
	vec3& translation,
	quat& rotation,
	vec3& scale);

extern bool verticesIntersection(
	const vec3& a,
	const vec3& b,
	const vec3& d1,
	const vec3& d2,
	vec3& intersection);

template<typename Integral>
constexpr bool isAligned(Integral x, u64 a) noexcept
{
	return (x & (Integral(a) - 1)) == 0;
}

template<typename Integral>
constexpr Integral alignUp(Integral x, u64 a) noexcept
{
	return Integral((x + (Integral(a) - 1)) & ~Integral(a - 1));
}

template<typename Integral>
constexpr Integral alignDown(Integral x, u64 a) noexcept
{
	return Integral(x & ~Integral(a - 1));
}

} // namespace erm::math
