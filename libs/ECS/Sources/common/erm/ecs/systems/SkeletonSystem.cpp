#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/AnimationSystem.h"

#include <erm/assets/data_structs/Skeleton.h>
#include <erm/assets/AssetsLib.h>
#include <erm/assets/AssetsRepo.h>

#include <erm/utils/Profiler.h>

namespace erm::ecs {

ERM_SYSTEM_IMPL(Skeleton)

void SkeletonSystem::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

	for (ID i = ROOT_ID; i < MAX_ID; ++i)
	{
		SkeletonComponent* skeletonComponent = GetComponent(i);
		if (!skeletonComponent || !skeletonComponent->IsDirty())
			continue;

		StringID skeletonID = skeletonComponent->GetSkeletonID();
		auto* skeleton = gAssetsLib.GetAssetsRepo().GetAsset<Skeleton>(skeletonID);

		if (skeleton == nullptr)
		{
			skeletonComponent->SetDirty(false);
			continue;
		}

		BonesTree& rootBone = skeleton->mRootBone;
		rootBone.ForEachDo(
			[](BonesTree& node) {
				BonesTree* parent = node.GetParent();
				Bone& bone = node.GetPayload();
				bone.mWorldTransform = parent ? parent->GetPayload().mWorldTransform : glm::identity<mat4>();
				bone.mWorldTransform *= bone.mLocalTransform;
				bone.mAnimatedTransform = bone.mWorldTransform * bone.mInverseBindTransform;
			});

		skeletonComponent->SetDirty(false);
	}
}

} // namespace erm::ecs
