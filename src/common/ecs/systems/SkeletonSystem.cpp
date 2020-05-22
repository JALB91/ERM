#include "erm/ecs/systems/SkeletonSystem.h"

#include "erm/utils/Profiler.h"

namespace erm::ecs {
	
	void SkeletonSystem::OnPostUpdate()
	{
		PROFILE_FUNCTION();
		
		for (ID i = ROOT_ID; i < MAX_ID; ++i)
		{
			SkeletonComponent* skeletonComponent = GetComponent(i);
			if (!skeletonComponent || !skeletonComponent->IsDirty()) continue;
			
			BonesTree* rootBone = skeletonComponent->mRootBone;
			if (!rootBone) continue;
			
			rootBone->ForEachDo(
				[](BonesTree& bone) {
					bone.GetPayload()->mAnimatedTransform = bone.GetParent() ? bone.GetParent()->GetPayload()->mAnimatedTransform : glm::identity<math::mat4>();
					bone.GetPayload()->mAnimatedTransform *= bone.GetPayload()->mLocalTransform;
				},
				[](BonesTree& bone) {
					bone.GetPayload()->mAnimatedTransform *= bone.GetPayload()->mInverseBindTransform;
				}
			);
			
			skeletonComponent->SetDirty(false);
		}
	}
	
}
