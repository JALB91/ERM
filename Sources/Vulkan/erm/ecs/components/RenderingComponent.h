#pragma once

#include "erm/ecs/IComponent.h"

// clang-format off
#ifndef ERM_RAY_TRACING_ENABLED
#include "erm/rendering/data_structs/RenderData.h"

#include <vector>
#else
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
		RenderingComponent() = default;

#ifdef ERM_RAY_TRACING_ENABLED
		SENSIBLE_MEMBER(CustomIndex, std::optional<uint32_t>, mCustomIndex)
#endif

	private:
#ifndef ERM_RAY_TRACING_ENABLED
		std::vector<RenderData> mRenderData;
#else
		std::optional<uint32_t> mCustomIndex;
		std::unique_ptr<DeviceBuffer> mTransformITBuffer;
#endif
	};

} // namespace erm::ecs
