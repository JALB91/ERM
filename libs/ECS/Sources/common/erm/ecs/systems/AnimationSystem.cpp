#include "erm/ecs/systems/AnimationSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/SkeletonSystem.h"

#include <erm/assets/AssetsLib.h>
#include <erm/assets/AssetsRepo.h>
#include <erm/assets/animations/SkeletonAnimation.h>
#include <erm/assets/data_structs/Skeleton.h>

#include <erm/utils/Profiler.h>

#include <cmath>

namespace erm::ecs {

ERM_SYSTEM_IMPL(Animation)

AnimationSystem::AnimationSystem(ECS& ecs)
	: ISystem(ecs)
	, mFrameTime(0.0f)
{}

void AnimationSystem::OnUpdate(float dt)
{
	mFrameTime = dt;
}

void AnimationSystem::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

	for (ID i = ROOT_ID; i < MAX_ID; ++i)
	{
		AnimationComponent* animationComponent = GetComponent(i);
		SkeletonComponent* skeletonComponent = mECS.GetComponent<SkeletonComponent>(i);

		if (!animationComponent || !animationComponent->mPlaying || !skeletonComponent)
			continue;

		SkeletonAnimation* currentAnimation = animationComponent->mSkeletonAnimation;
		const auto skeletonID = skeletonComponent->GetSkeletonID();

		if (!currentAnimation || !skeletonID.IsValid() || currentAnimation->mTotalAnimationTime <= 0.01f || currentAnimation->mKeyFrames.empty())
			continue;

		auto* skeleton = gAssetsLib.GetAssetsRepo().GetAsset<Skeleton>(skeletonID);
		
		if (skeleton == nullptr)
			return;
		
		BonesTree& rootBone = skeleton->mRootBone;

		float& currentAnimationTime = animationComponent->mCurrentAnimationTime;
		const std::vector<KeyFrame>& keyFrames = currentAnimation->mKeyFrames;

		const KeyFrame* prevKeyFrame = &keyFrames[0];
		const KeyFrame* nextKeyFrame = &keyFrames[0];

		currentAnimationTime += mFrameTime * animationComponent->mTimeScale;

		if (currentAnimationTime > currentAnimation->mTotalAnimationTime)
		{
			currentAnimationTime = std::fmod(currentAnimationTime, currentAnimation->mTotalAnimationTime);
			ERM_ASSERT(!std::isnan(currentAnimationTime));
		}

		for (unsigned int j = 1; j < static_cast<unsigned int>(keyFrames.size()); ++j)
		{
			nextKeyFrame = &keyFrames[j];
			if (keyFrames[j].mTimestamp > currentAnimationTime)
			{
				break;
			}
			prevKeyFrame = &keyFrames[j];
		}
		
		ERM_ASSERT(nextKeyFrame->mTimestamp > prevKeyFrame->mTimestamp);

		const float progression = (currentAnimationTime - prevKeyFrame->mTimestamp) / (nextKeyFrame->mTimestamp - prevKeyFrame->mTimestamp);

		rootBone.ForEachDo(
			[&prevKeyFrame, &nextKeyFrame, progression](BonesTree& node) {
				Bone& currentBone = node.GetPayload();

				if (node.GetId() >= MAX_BONES)
					return;

				const Pose& prevPose = prevKeyFrame->mTransforms[node.GetId()];
				const Pose& nextPose = nextKeyFrame->mTransforms[node.GetId()];

				Pose currentPose;
				currentPose.mTranslation = glm::mix(prevPose.mTranslation, nextPose.mTranslation, progression);
				currentPose.mRotation = glm::normalize(glm::slerp(prevPose.mRotation, nextPose.mRotation, progression));
				currentPose.mScale = glm::mix(prevPose.mScale, nextPose.mScale, progression);

				currentBone.mLocalTransform = glm::translate(glm::identity<math::mat4>(), currentPose.mTranslation);
				currentBone.mLocalTransform *= glm::mat4_cast(currentPose.mRotation);
				currentBone.mLocalTransform = glm::scale(currentBone.mLocalTransform, currentPose.mScale);
			});

		skeletonComponent->SetDirty(true);
	}
}

} // namespace erm::ecs
