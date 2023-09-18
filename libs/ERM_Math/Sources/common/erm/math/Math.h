#pragma once

#include "erm/math/Types.h"

namespace erm::math {

void DecomposeMatrix(
	const mat4& matrix,
	vec3& translation,
	quat& rotation,
	vec3& scale);

bool VerticesIntersection(
	const vec3& a,
	const vec3& b,
	const vec3& d1,
	const vec3& d2,
	vec3& intersection);

template<typename Integral>
constexpr bool IsAligned(Integral x, u64 a) noexcept
{
	return (x & (Integral(a) - 1)) == 0;
}

template<typename Integral>
constexpr Integral AlignUp(Integral x, u64 a) noexcept
{
	return Integral((x + (Integral(a) - 1)) & ~Integral(a - 1));
}

template<typename Integral>
constexpr Integral AlignDown(Integral x, u64 a) noexcept
{
	return Integral(x & ~Integral(a - 1));
}

} // namespace erm::math
