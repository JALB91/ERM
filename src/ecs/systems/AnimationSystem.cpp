#include "ecs/systems/AnimationSystem.h"

#include "ecs/ECS.h"
#include "ecs/systems/SkeletonSystem.h"

#include "rendering/animations/SkeletonAnimation.h"

#include "utils/Profiler.h"

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
			PROFILE_FUNCTION();
			
			for (ID i = ROOT_ID; i < MAX_ID; ++i)
			{
				AnimationComponent* animationComponent = GetComponent(i);
				SkeletonComponent* skeletonComponent = mSkeletonSystem.GetComponent(i);
				
				if (!animationComponent || !animationComponent->mPlaying || !skeletonComponent) continue;
				
				SkeletonAnimation* currentAnimation = animationComponent->mSkeletonAnimation;
				BonesTree* rootBone = skeletonComponent->GetRootBone();
				
				if (!currentAnimation || !rootBone) continue;
				
				float& currentAnimationTime = animationComponent->mCurrentAnimationTime;
				const std::vector<KeyFrame>& keyFrames = currentAnimation->mKeyFrames;
				
				const KeyFrame* prevKeyFrame = &keyFrames[0];
				const KeyFrame* nextKeyFrame = &keyFrames[0];
				
				currentAnimationTime += mFrameTime * animationComponent->mTimeScale;
				
				if (currentAnimationTime > currentAnimation->mTotalAnimationTime)
				{
					currentAnimationTime = std::fmod(currentAnimationTime, currentAnimation->mTotalAnimationTime);
				}
				
				for (unsigned int i = 1; i < static_cast<unsigned int>(keyFrames.size()); ++i)
				{
					nextKeyFrame = &keyFrames[i];
					if (keyFrames[i].mTimestamp > currentAnimationTime)
					{
						break;
					}
					prevKeyFrame = &keyFrames[i];
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
					}
				);
				
				skeletonComponent->SetDirty(true);
			}
		}
		
	}
}
