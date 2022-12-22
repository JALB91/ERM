#pragma once

#include "erm/math/vec.h"

#include "erm/rendering/data_structs/SubpassData.h"

namespace erm {

struct RenderConfigs
{
	static const RenderConfigs DEFAULT_RENDER_CONFIGS;

	RenderConfigs(SubpassData&& subpassData, math::vec4&& clearColor);

	bool IsRenderPassLevelCompatible(const RenderConfigs& other) const;

	SubpassData mSubpassData;
	math::vec4 mClearColor;
};

} // namespace erm
