#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/RenderData.h"

#include <vector>

namespace erm {
	struct Material;
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

		std::vector<RenderData> mRenderData;
		std::map<Material*, size_t> mMaterialIndices;
	};

} // namespace erm::ecs
