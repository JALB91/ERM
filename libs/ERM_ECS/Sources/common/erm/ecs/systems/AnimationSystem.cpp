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

void AnimationSystem::update(float dt)
{
	mFrameTime = dt;
}

void AnimationSystem::postUpdate()
{
	ERM_PROFILE_FUNCTION();

	for (ID i = ROOT_ID; i < MAX_ID; ++i)
	{
		auto* animationComponent = getComponent(i);
		auto* skeletonComponent = mECS.getComponent<SkeletonComponent>(i);

		if (!animationComponent || !animationComponent->mPlaying || !skeletonComponent)
		{
			continue;
		}

		auto* currentAnimation = animationComponent->mSkeletonAnimation;
		const auto skeletonID = skeletonComponent->getSkeletonID();

		if (!currentAnimation || !skeletonID.isValid() || currentAnimation->mTotalAnimationTime <= 0.01f || currentAnimation->mKeyFrames.empty())
		{
			continue;
		}

		auto* skeleton = gAssetsLib.getAssetsRepo().getAsset<Skeleton>(skeletonID);
		
		if (skeleton == nullptr)
		{
			return;
		}
		
		auto& rootBone = skeleton->mRootBone;

		float& currentAnimationTime = animationComponent->mCurrentAnimationTime;
		const auto& keyFrames = currentAnimation->mKeyFrames;

		const auto* prevKeyFrame = &keyFrames[0];
		const auto* nextKeyFrame = &keyFrames[0];

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

		rootBone.forEachDo(
			[&prevKeyFrame, &nextKeyFrame, progression](BonesTree& node) {
				auto& currentBone = node.getPayload();

				if (node.getId() >= MAX_BONES)
					return;

				const auto& prevPose = prevKeyFrame->mTransforms[node.getId()];
				const auto& nextPose = nextKeyFrame->mTransforms[node.getId()];

				Pose currentPose;
				currentPose.mTranslation = glm::mix(prevPose.mTranslation, nextPose.mTranslation, progression);
				currentPose.mRotation = glm::normalize(glm::slerp(prevPose.mRotation, nextPose.mRotation, progression));
				currentPose.mScale = glm::mix(prevPose.mScale, nextPose.mScale, progression);

				currentBone.mLocalTransform = glm::translate(glm::identity<mat4>(), currentPose.mTranslation);
				currentBone.mLocalTransform *= glm::mat4_cast(currentPose.mRotation);
				currentBone.mLocalTransform = glm::scale(currentBone.mLocalTransform, currentPose.mScale);
			});

		skeletonComponent->setDirty(true);
	}
}

} // namespace erm::ecs
