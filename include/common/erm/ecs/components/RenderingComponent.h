#pragma once

#include "erm/ecs/IComponent.h"

namespace erm {
	namespace ecs {
		
		class RenderingSystem;
		
		struct RenderingComponent : public IComponent
		{
		public:
			typedef RenderingSystem SYSTEM_TYPE;
			friend class RenderingSystem;
		
		};
		
	}
}

