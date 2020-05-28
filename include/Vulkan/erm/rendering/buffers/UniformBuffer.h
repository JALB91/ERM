#pragma once

#include "erm/math/mat.h"

namespace erm {

	struct UniformBuffer
	{
		alignas(16) erm::math::mat4 mMVP;
	};

} // namespace erm
