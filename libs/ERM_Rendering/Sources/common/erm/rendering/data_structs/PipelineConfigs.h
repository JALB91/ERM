#pragma once

#include "erm/rendering/data_structs/BindingConfigs.h"
#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/enums/PolygonMode.h"

#include <erm/utils/Utils.h>
#include <erm/utils/StringID.h>

namespace erm {
class IShaderProgram;
}

namespace erm {

struct PipelineConfigs : public BindingConfigs
{
	static const PipelineConfigs DEFAULT;

	virtual ~PipelineConfigs() = default;

	bool isPipelineLevelCompatible(const PipelineConfigs& other) const;

	bool isShaderCompatible(const PipelineConfigs& other) const;
	bool isBlendCompatible(const PipelineConfigs& other) const;
	bool isCullCompatible(const PipelineConfigs& other) const;
	bool isDepthCompatible(const PipelineConfigs& other) const;
	bool isDrawModeCompatible(const PipelineConfigs& other) const;
	bool isPolyModeCompatible(const PipelineConfigs& other) const;

	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(CullMode, CullMode, CullMode::BACK);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(FrontFace, FrontFace, FrontFace::CCW);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(BlendEnabled, bool, true);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(BlendFunction, BlendFunction, BlendFunction::ONE_MINUS_SRC_ALPHA);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(DepthTestEnabled, bool, true);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(DepthWriteEnabled, bool, true);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(DepthFunction, DepthFunction, DepthFunction::LESS);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(DrawMode, DrawMode, DrawMode::TRIANGLES);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(PolygonMode, PolygonMode, PolygonMode::FILL);
	StringID mShaderProgram = StringID::INVALID;
};

} // namespace erm
