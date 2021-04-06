#pragma once

#include "erm/rendering/data_structs/SubpassData.h"

namespace erm {

struct RenderConfigs
{
	static const RenderConfigs DEFAULT_RENDER_CONFIGS;

	RenderConfigs(const SubpassData& subpassData);

	bool operator==(const RenderConfigs& other) const;
	bool operator!=(const RenderConfigs& other) const;

	bool IsRenderPassLevelCompatible(const RenderConfigs& other) const;

	bool IsSubpassCompatible(const RenderConfigs& other) const;

	SubpassData mSubpassData;
};

} // namespace erm
