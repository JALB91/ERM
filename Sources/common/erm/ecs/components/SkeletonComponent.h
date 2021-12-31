#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/Skin.h"

namespace erm::ecs {
class SkeletonSystem;
}

namespace erm::ecs {

struct SkeletonComponent : public IComponent
{
public:
	typedef SkeletonSystem SYSTEM_TYPE;
	friend class SkeletonSystem;

public:
	SkeletonComponent(Skin* skin = nullptr)
		: mSkin(skin)
	{}

	ERM_SENSIBLE_MEMBER(Skin, Skin*, nullptr);
	ERM_SENSIBLE_MEMBER(DisplayBones, bool, false);
};

} // namespace erm::ecs
