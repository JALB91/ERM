#pragma once

#include "erm/math/mat.h"
#include "erm/math/quat.h"
#include "erm/math/vec.h"

namespace erm::math {

void DecomposeMatrix(
	const math::mat4& matrix,
	math::vec3& translation,
	math::quat& rotation,
	math::vec3& scale);

bool VerticesIntersection(
	const math::vec3& a,
	const math::vec3& b,
	const math::vec3& d1,
	const math::vec3& d2,
	math::vec3& intersection);

template<class integral>
constexpr bool is_aligned(integral x, size_t a) noexcept
{
	return (x & (integral(a) - 1)) == 0;
}

template<class integral>
constexpr integral align_up(integral x, size_t a) noexcept
{
	return integral((x + (integral(a) - 1)) & ~integral(a - 1));
}

template<class integral>
constexpr integral align_down(integral x, size_t a) noexcept
{
	return integral(x & ~integral(a - 1));
}

} // namespace erm::math
