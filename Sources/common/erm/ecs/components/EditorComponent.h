#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/PipelineConfigs.h"
#include "erm/rendering/data_structs/RenderData.h"

namespace erm::ecs {
class EditorSystem;
}

namespace erm::ecs {

struct EditorComponent : public IComponent
{
public:
	typedef EditorSystem SYSTEM_TYPE;
	friend class EditorSystem;

public:
	EditorComponent(const PipelineConfigs& bonesPipelineConfig = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS)
		: mBonesRenderData(bonesPipelineConfig)
	{}

	SENSIBLE_MEMBER(IsSelected, bool, false)

private:
	RenderData mBonesRenderData;
};

} // namespace erm::ecs
