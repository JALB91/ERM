#pragma once

#include "erm/rendering/data_structs/BindingConfigs.h"
#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/enums/PolygonMode.h"

#include "erm/utils/Utils.h"

namespace erm {
	class ShaderProgram;
}

namespace erm {

	struct PipelineConfigs : public BindingConfigs
	{
		static const PipelineConfigs MODELS_PIPELINE_CONFIGS;

		PipelineConfigs();
		virtual ~PipelineConfigs() = default;

		bool IsPipelineLevelCompatible(const PipelineConfigs& other) const;

		bool IsShaderCompatible(const PipelineConfigs& other) const;
		bool IsBlendCompatible(const PipelineConfigs& other) const;
		bool IsCullCompatible(const PipelineConfigs& other) const;
		bool IsDepthCompatible(const PipelineConfigs& other) const;
		bool IsDrawModeCompatible(const PipelineConfigs& other) const;
		bool IsPolyModeCompatible(const PipelineConfigs& other) const;

		DECL_SET_GET_OPT_WITH_DEFAULT(CullMode, CullMode, CullMode::BACK);
		DECL_SET_GET_OPT_WITH_DEFAULT(FrontFace, FrontFace, FrontFace::CCW);
		DECL_SET_GET_OPT_WITH_DEFAULT(BlendEnabled, bool, true);
		DECL_SET_GET_OPT_WITH_DEFAULT(BlendFunction, BlendFunction, BlendFunction::ONE_MINUS_SRC_ALPHA);
		DECL_SET_GET_OPT_WITH_DEFAULT(DepthTestEnabled, bool, true);
		DECL_SET_GET_OPT_WITH_DEFAULT(DepthWriteEnabled, bool, true);
		DECL_SET_GET_OPT_WITH_DEFAULT(DepthFunction, DepthFunction, DepthFunction::LESS);
		DECL_SET_GET_OPT_WITH_DEFAULT(DrawMode, DrawMode, DrawMode::TRIANGLES);
		DECL_SET_GET_OPT_WITH_DEFAULT(PolygonMode, PolygonMode, PolygonMode::FILL);
		ShaderProgram* mShaderProgram;
	};

} // namespace erm