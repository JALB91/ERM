#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/RenderData.h"

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
		std::vector<RenderData> mRenderData;
	};

} // namespace erm::ecs
