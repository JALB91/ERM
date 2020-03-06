#pragma once

#include "erm/math/mat.h"
#include "erm/math/quat.h"
#include "erm/math/vec.h"

namespace erm {
	namespace math {
		
		void DecomposeMatrix(
			const math::mat4& matrix,
			math::vec3& translation,
			math::quat& rotation,
			math::vec3& scale
		);

		bool VerticesIntersection(
			const math::vec3& a,
			const math::vec3& b,
			const math::vec3& d1,
			const math::vec3& d2,
			math::vec3& intersection
		);

	}
}
