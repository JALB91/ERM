#include "erm/ecs/systems/AnimationSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/SkeletonSystem.h"

#include "erm/rendering/animations/SkeletonAnimation.h"

#include "erm/utils/Profiler.h"

#include <cmath>

namespace erm::ecs {

AnimationSystem::AnimationSystem(ECS& ecs)
	: ISystem(ecs)
{}

void AnimationSystem::Init()
{
	mSkeletonSystem = &mECS.GetSystem<SkeletonSystem>();
}

void AnimationSystem::OnUpdate(float dt)
{
	mFrameTime = dt;
}

void AnimationSystem::OnPostUpdate()
{
	PROFILE_FUNCTION();

	for (ID i = ROOT_ID; i < MAX_ID; ++i)
	{
		AnimationComponent* animationComponent = GetComponent(i);
		SkeletonComponent* skeletonComponent = mSkeletonSystem->GetComponent(i);

		if (!animationComponent || !animationComponent->mPlaying || !skeletonComponent)
			continue;

		SkeletonAnimation* currentAnimation = animationComponent->mSkeletonAnimation;
		Skin* skin = skeletonComponent->GetSkin();

		if (!currentAnimation || !skin)
			continue;

		BonesTree* rootBone = skin->mRootBone.get();

		float& currentAnimationTime = animationComponent->mCurrentAnimationTime;
		const std::vector<KeyFrame>& keyFrames = currentAnimation->mKeyFrames;

		const KeyFrame* prevKeyFrame = &keyFrames[0];
		const KeyFrame* nextKeyFrame = &keyFrames[0];

		currentAnimationTime += mFrameTime * animationComponent->mTimeScale;

		if (currentAnimationTime > currentAnimation->mTotalAnimationTime)
		{
			currentAnimationTime = std::fmod(currentAnimationTime, currentAnimation->mTotalAnimationTime);
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

		const float progression = (currentAnimationTime - prevKeyFrame->mTimestamp) / (nextKeyFrame->mTimestamp - prevKeyFrame->mTimestamp);

		rootBone->ForEachDo(
			[&prevKeyFrame, &nextKeyFrame, progression](BonesTree& node) {
				Bone& currentBone = *node.GetPayload();

				const Pose& prevPose = prevKeyFrame->mTransforms[node.GetId()];
				const Pose& nextPose = nextKeyFrame->mTransforms[node.GetId()];

				Pose currentPose;
				currentPose.mTranslation = glm::mix(prevPose.mTranslation, nextPose.mTranslation, progression);
				currentPose.mRotation = glm::normalize(glm::slerp(prevPose.mRotation, nextPose.mRotation, progression));
				currentPose.mScale = glm::mix(prevPose.mScale, nextPose.mScale, progression);

				currentBone.mLocalTransform = glm::identity<math::mat4>();
				currentBone.mLocalTransform = glm::translate(currentBone.mLocalTransform, currentPose.mTranslation);
				currentBone.mLocalTransform *= glm::mat4_cast(currentPose.mRotation);
				currentBone.mLocalTransform = glm::scale(currentBone.mLocalTransform, currentPose.mScale);
			});

		skeletonComponent->SetDirty(true);
	}
}

} // namespace erm::ecs
