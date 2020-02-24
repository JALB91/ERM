#include "ecs/systems/AnimationSystem.h"

#include "ecs/ECS.h"
#include "ecs/systems/SkeletonSystem.h"

#include <cmath>

namespace erm {
	namespace ecs {
		
		AnimationSystem::AnimationSystem(ECS& ecs)
			: ISystem(ecs)
			, mSkeletonSystem(mECS.GetSystem<SkeletonSystem>())
		{}
		
		void AnimationSystem::OnUpdate(float dt)
		{
			mFrameTime = dt;
		}
		
		void AnimationSystem::OnPostUpdate()
		{
			for (ID i = ROOT_ID; i < MAX_ID; ++i)
			{
				AnimationComponent* animationComponent = GetComponent(i);
				SkeletonComponent* skeletonComponent = mSkeletonSystem.GetComponent(i);
				
				if (!animationComponent || !skeletonComponent) continue;
				
				float& currentAnimationTime = animationComponent->mSkeletonAnimation.mCurrentAnimationTime;
				std::vector<KeyFrame>& keyFrames = animationComponent->mSkeletonAnimation.mKeyFrames;
				
				KeyFrame* prevKeyFrame = nullptr;
				KeyFrame* nextKeyFrame = nullptr;
				
				currentAnimationTime += mFrameTime;
				
				if (currentAnimationTime > animationComponent->mSkeletonAnimation.mTotalAnimationTime)
				{
					currentAnimationTime = std::fmod(currentAnimationTime, animationComponent->mSkeletonAnimation.mTotalAnimationTime);
				}
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(keyFrames.size()); ++i)
				{
					KeyFrame& keyFrame = keyFrames[i];
					if (keyFrame.mTimestamp > currentAnimationTime)
					{
						prevKeyFrame = &keyFrame;
						nextKeyFrame = &keyFrames[i + 1];
					}
				}
				
				const float weigth = (currentAnimationTime - prevKeyFrame->mTimestamp) / nextKeyFrame->mTimestamp - prevKeyFrame->mTimestamp;
				
				BonesTree::ForEachDo(skeletonComponent->mRootBone, [&prevKeyFrame, &nextKeyFrame, weigth](BonesTree& node) {
					Bone& currentBone = *node.GetPayload();
					Pose currentPose;
					
					const Pose& prevPose = prevKeyFrame->mTransforms[currentBone.mId];
					const Pose& nextPose = nextKeyFrame->mTransforms[currentBone.mId];
					
					currentPose.mRotation = glm::mix(prevPose.mRotation, nextPose.mRotation, weigth);
					currentPose.mTranslation = glm::mix(prevPose.mTranslation, nextPose.mTranslation, weigth);
					currentPose.mScale = glm::mix(prevPose.mScale, nextPose.mScale, weigth);
					
					currentBone.mAnimatedTransform = node.GetParent() ? node.GetParent()->GetPayload()->mAnimatedTransform : glm::identity<math::mat4>();
					currentBone.mAnimatedTransform = glm::translate(currentBone.mBindTransform, currentPose.mTranslation);
					currentBone.mAnimatedTransform = glm::mat4_cast(currentPose.mRotation) * currentBone.mAnimatedTransform;
					currentBone.mAnimatedTransform = glm::scale(currentBone.mAnimatedTransform, currentPose.mScale);
				});
				
				BonesTree::ForEachDo(skeletonComponent->mRootBone, [](BonesTree& node) {
					Bone& currentBone = *node.GetPayload();
					currentBone.mAnimatedTransform *= currentBone.mInverseBindTransform;
				});
			}
		}
		
	}
}
