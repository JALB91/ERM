#pragma once

#include <cstdint>

namespace erm {
namespace ecs {

typedef uint32_t ID;

static constexpr ID ROOT_ID = 0;
static constexpr ID MAX_ID = 1000;
static constexpr ID INVALID_ID = MAX_ID;

} // namespace ecs
} // namespace erm
