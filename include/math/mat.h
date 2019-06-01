#pragma once

#include <glm/glm.hpp>

namespace erm {
	
namespace math {
	
	template <int R, int C, typename S>
	using mat = glm::mat<R, C, S>;
	
	typedef mat<2, 2, float> mat2;
	typedef mat<3, 3, float> mat3;
	typedef mat<4, 4, float> mat4;
	
}

}
