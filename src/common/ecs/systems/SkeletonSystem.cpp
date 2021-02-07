#include "erm/ecs/systems/SkeletonSystem.h"

#include "erm/utils/Profiler.h"

namespace erm::ecs {

	void SkeletonSystem::OnPostUpdate()
	{
		PROFILE_FUNCTION();

		for (ID i = ROOT_ID; i < MAX_ID; ++i)
		{
			SkeletonComponent* skeletonComponent = GetComponent(i);
			if (!skeletonComponent || !skeletonComponent->IsDirty())
				continue;

			Skin* skin = skeletonComponent->GetSkin();

			if (!skin)
				continue;

			BonesTree* rootBone = skin->mRootBone.get();

			rootBone->ForEachDo(
				[](BonesTree& bone) {
					bone.GetPayload()->mWorldTransform = bone.GetParent() ? bone.GetParent()->GetPayload()->mWorldTransform : glm::identity<math::mat4>();
					bone.GetPayload()->mWorldTransform *= bone.GetPayload()->mLocalTransform;
				},
				[](BonesTree& bone) {
					bone.GetPayload()->mAnimatedTransform = bone.GetPayload()->mWorldTransform * bone.GetPayload()->mInverseBindTransform;
				});

			skeletonComponent->SetDirty(false);
		}
	}

} // namespace erm::ecs
