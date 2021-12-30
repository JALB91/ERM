#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/RenderData.h"

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

struct RenderingComponent : public IComponent
{
public:
	typedef RenderingSystem SYSTEM_TYPE;
	friend class RenderingSystem;

public:
#ifdef ERM_RAY_TRACING_ENABLED
	SENSIBLE_MEMBER(CustomIndex, std::optional<uint32_t>, {})
	SENSIBLE_MEMBER(UseRayTracing, bool, true)
#endif

private:
	std::vector<RenderData> mRenderData;
#ifdef ERM_RAY_TRACING_ENABLED
	std::unique_ptr<DeviceBuffer> mInstanceDataBuffer;
#endif
};

} // namespace erm::ecs
