#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/AnimationSystem.h"

#include <erm/assets/data_structs/Skeleton.h>
#include <erm/assets/AssetsLib.h>
#include <erm/assets/AssetsRepo.h>

#include <erm/utils/Profiler.h>

namespace erm::ecs {

ERM_SYSTEM_IMPL(Skeleton)

void SkeletonSystem::postUpdate()
{
	ERM_PROFILE_FUNCTION();

	for (ID i = ROOT_ID; i < MAX_ID; ++i)
	{
		auto* skeletonComponent = getComponent(i);
		if (!skeletonComponent || !skeletonComponent->isDirty())
		{
			continue;
		}

		auto skeletonID = skeletonComponent->getSkeletonID();
		auto* skeleton = gAssetsLib.getAssetsRepo().getAsset<Skeleton>(skeletonID);

		if (skeleton == nullptr)
		{
			skeletonComponent->setDirty(false);
			continue;
		}

		auto& rootBone = skeleton->mRootBone;
		rootBone.forEachDo(
			[](BonesTree& node) {
				auto* parent = node.getParent();
				auto& bone = node.getPayload();
				bone.mWorldTransform = parent ? parent->getPayload().mWorldTransform : glm::identity<mat4>();
				bone.mWorldTransform *= bone.mLocalTransform;
				bone.mAnimatedTransform = bone.mWorldTransform * bone.mInverseBindTransform;
			});

		skeletonComponent->setDirty(false);
	}
}

} // namespace erm::ecs
