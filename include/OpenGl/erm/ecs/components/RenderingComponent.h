#pragma once

#include "erm/ecs/IComponent.h"
#include "erm/rendering/RenderPassConfig.h"

namespace erm::ecs {
	class RenderingSystem;
}

namespace erm::ecs {

	struct RenderingComponent : public IComponent
	{
	public:
		typedef RenderingSystem SYSTEM_TYPE;
		friend class RenderingSystem;

		SENSIBLE_MEMBER(RenderPassConfig, RenderPassConfig, mRenderPassConfig);

	private:
		RenderPassConfig mRenderPassConfig;
	};

} // namespace erm::ecs
