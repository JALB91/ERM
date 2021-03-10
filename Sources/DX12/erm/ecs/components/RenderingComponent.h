#pragma once

#include "erm/ecs/IComponent.h"

namespace erm::ecs {
	class RenderingSystem;
}

namespace erm::ecs {

	struct RenderingComponent : public IComponent
	{
	public:
		typedef RenderingSystem SYSTEM_TYPE;
		friend class RenderingSystem;

	public:
	private:
	};

} // namespace erm::ecs