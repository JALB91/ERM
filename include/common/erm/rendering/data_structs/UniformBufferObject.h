#pragma once

#include "erm/math/mat.h"

namespace erm {

	typedef uint8_t UboId;

	struct IUbo
	{};

	struct UboBasic : public IUbo
	{
		static constexpr UboId ID = 0;

		alignas(16) erm::math::mat4 mMVP;
	};

} // namespace erm
