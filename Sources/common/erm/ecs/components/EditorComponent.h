#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/RenderConfigs.h"
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
	EditorComponent(const RenderConfigs& bonesRenderConfig = RenderConfigs::MODELS_RENDER_CONFIGS)
		: mIsSelected(false)
		, mBonesRenderData(bonesRenderConfig)
	{}

	SENSIBLE_MEMBER(IsSelected, bool, mIsSelected)

private:
	bool mIsSelected;
	RenderData mBonesRenderData;
};

} // namespace erm::ecs