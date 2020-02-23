#pragma once

#include "math/vec.h"

namespace erm {
	
namespace math {

	bool VerticesIntersection(
		const math::vec3& a,
		const math::vec3& b,
		const math::vec3& d1,
		const math::vec3& d2,
		math::vec3& intersection
	);

}
	
}
