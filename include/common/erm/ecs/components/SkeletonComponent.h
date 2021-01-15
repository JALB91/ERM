#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/Bone.h"

#include <memory>

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
		SkeletonComponent(BonesTree* rootBone = nullptr)
			: mRootBone(rootBone ? std::make_unique<BonesTree>(rootBone->Clone()) : nullptr)
		{}

		inline void SetRootBone(BonesTree* rootBone)
		{
			if ((!rootBone && !mRootBone) || (rootBone && mRootBone && mRootBone->Equal(*rootBone)))
				return;
			mRootBone = rootBone ? std::make_unique<BonesTree>(rootBone->Clone()) : nullptr;
			SetDirty(true);
		}

		inline BonesTree* GetRootBone()
		{
			return mRootBone.get();
		}

	private:
		std::unique_ptr<BonesTree> mRootBone;
	};

} // namespace erm::ecs
