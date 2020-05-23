#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/Bone.h"

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
		SkeletonComponent(BonesTree* tree = nullptr)
			: mRootBone(tree)
		{}

		SENSIBLE_MEMBER(RootBone, BonesTree*, mRootBone);

	private:
		BonesTree* mRootBone;
	};

} // namespace erm::ecs
