#pragma once

#include <cstddef>

namespace erm {
	namespace ecs {

		typedef unsigned int ID;

		static constexpr ID ROOT_ID = 0;
		static constexpr ID MAX_ID = 100;
		static constexpr ID INVALID_ID = MAX_ID;

	} // namespace ecs
} // namespace erm
