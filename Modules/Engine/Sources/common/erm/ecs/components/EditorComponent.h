#pragma once

#include "erm/ecs/Component.h"

#include "erm/rendering/data_structs/RenderData.h"

namespace erm::ecs {
class EditorSystem;
}

namespace erm::ecs {

struct EditorComponent
{
	ERM_COMPONENT_DECL(Editor)

public:
	EditorComponent(
		const RenderConfigs& renderConfigs = RenderConfigs::DEFAULT_RENDER_CONFIGS,
		const PipelineConfigs& pipelineConfig = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS)
		: mBonesRenderData(renderConfigs, pipelineConfig)
	{}

	ERM_SENSIBLE_MEMBER(IsSelected, bool, false)

private:
	RenderData mBonesRenderData;
};

} // namespace erm::ecs
