#pragma once

#include <cstdint>

namespace erm::ecs {

using ID = uint32_t;

static constexpr ID ROOT_ID = 0;
static constexpr ID MAX_ID = 1000;
static constexpr ID INVALID_ID = MAX_ID;

static constexpr ID TransformSystemId = 0;
static constexpr ID LightSystemId = 1;
static constexpr ID SkeletonSystemId = 2;
static constexpr ID AnimationSystemId = 3;
static constexpr ID ModelSystemId = 4;
static constexpr ID CameraSystemId = 5;
static constexpr ID RenderingSystemId = 6;

} // namespace erm::ecs
