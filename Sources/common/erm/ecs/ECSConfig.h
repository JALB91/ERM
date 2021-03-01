#pragma once

#include <cstdint>

namespace erm {
	namespace ecs {

		typedef uint32_t ID;

		static constexpr ID ROOT_ID = 0;
		static constexpr ID MAX_ID = 100;
		static constexpr ID INVALID_ID = MAX_ID;

	} // namespace ecs
} // namespace erm
