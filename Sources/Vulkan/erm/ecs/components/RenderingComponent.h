#pragma once

#include "erm/ecs/IComponent.h"

// clang-format off
#ifndef ERM_RAY_TRACING_ENABLED
#include "erm/rendering/data_structs/RenderData.h"
#else
#include "erm/ray_tracing/RTRenderData.h"
#endif
// clang-format on

#include <vector>

namespace erm::ecs {
	class RenderingSystem;
} // namespace erm::ecs

namespace erm::ecs {

	struct RenderingComponent : public IComponent
	{
	public:
		typedef RenderingSystem SYSTEM_TYPE;
		friend class RenderingSystem;

	public:
		RenderingComponent() = default;

	private:
#ifndef ERM_RAY_TRACING_ENABLED
		std::vector<RenderData> mRenderData;
#else
		RTRenderData mRTRenderData;
#endif
	};

} // namespace erm::ecs
