#pragma once

#include "erm/math/Types.h"

#include "erm/rendering/data_structs/SubpassData.h"

namespace erm {

struct RenderConfigs
{
	static const RenderConfigs DEFAULT;

	RenderConfigs(SubpassData&& subpassData, vec4&& clearColor);

	bool IsRenderPassLevelCompatible(const RenderConfigs& other) const;

	SubpassData mSubpassData;
	vec4 mClearColor;
};

} // namespace erm
