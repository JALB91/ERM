#pragma once

#include "erm/math/BoundingBox.h"

#include "erm/rendering/data_structs/SubpassData.h"
#include "erm/rendering/data_structs/PipelineConfigs.h"

#include "erm/utils/Utils.h"

namespace erm {

	struct RenderConfigs : public PipelineConfigs
	{
		static const RenderConfigs MODELS_RENDER_CONFIGS;

		RenderConfigs(const SubpassData& subpassData);

		bool operator==(const RenderConfigs& other) const;
		bool operator!=(const RenderConfigs& other) const;

		bool IsRenderPassLevelCompatible(const RenderConfigs& other) const;

		bool IsSubpassCompatible(const RenderConfigs& other) const;

		SubpassData mSubpassData;
		DECL_SET_GET_OPT_WITH_DEFAULT(NormViewport, BoundingBox2D, BoundingBox2D({0.0f, 0.0f}, {1.0f, 1.0f}));
	};

} // namespace erm
