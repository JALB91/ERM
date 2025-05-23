#pragma once

#include "erm/ecs/Component.h"

#include <erm/math/Types.h>
#include <erm/rendering/data_structs/RenderData.h>

#include <vector>
// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include <optional>
#include <memory>
#endif
// clang-format on

namespace erm {
class DeviceBuffer;
namespace ecs {
class RenderingSystem;
}
} // namespace erm

namespace erm::ecs {

struct RenderingComponent
{
	ERM_COMPONENT_DECL(Rendering)

public:
	ERM_SENSIBLE_MEMBER(RenderData, std::vector<RenderData>);
#ifdef ERM_RAY_TRACING_ENABLED
	ERM_SENSIBLE_MEMBER(CustomIndex, std::optional<u32>)
	ERM_SENSIBLE_MEMBER(UseRayTracing, bool, true)
#endif

};

} // namespace erm::ecs
