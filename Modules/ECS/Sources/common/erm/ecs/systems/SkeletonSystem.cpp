#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/AnimationSystem.h"

#include <erm/resources/data_structs/Skin.h>

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

		Skin* skin = skeletonComponent->GetSkin();

		if (!skin)
		{
			skeletonComponent->SetDirty(false);
			continue;
		}

		BonesTree* rootBone = skin->mRootBone.get();

		rootBone->ForEachDo(
			[](BonesTree& node) {
				BonesTree* parent = node.GetParent();
				Bone& bone = node.GetPayload();
				bone.mWorldTransform = parent ? parent->GetPayload().mWorldTransform : glm::identity<math::mat4>();
				bone.mWorldTransform *= bone.mLocalTransform;
				bone.mAnimatedTransform = bone.mWorldTransform * bone.mInverseBindTransform;
			});

		skeletonComponent->SetDirty(false);
	}
}

} // namespace erm::ecs
