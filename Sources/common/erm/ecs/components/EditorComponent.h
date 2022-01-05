#pragma once

#include "erm/ecs/Component.h"

#include "erm/rendering/data_structs/PipelineConfigs.h"
#include "erm/rendering/data_structs/RenderData.h"

namespace erm::ecs {
class EditorSystem;
}

namespace erm::ecs {

struct EditorComponent
{
	ERM_COMPONENT_DECL(Editor)

public:
	EditorComponent(const PipelineConfigs& bonesPipelineConfig = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS)
		: mBonesRenderData(bonesPipelineConfig)
	{}

	ERM_SENSIBLE_MEMBER(IsSelected, bool, false)

private:
	RenderData mBonesRenderData;
};

} // namespace erm::ecs
