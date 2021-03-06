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
		, mDisplayBones(false)
	{}

	SENSIBLE_MEMBER(Skin, Skin*, mSkin);
	SENSIBLE_MEMBER(DisplayBones, bool, mDisplayBones);

private:
	Skin* mSkin;
	bool mDisplayBones;
};

} // namespace erm::ecs
