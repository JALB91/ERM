#include "erm/loaders/fbx/FBXSkeletonLoader.h"

namespace erm {

	void ParseSkeleton(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		FbxSkeleton* skeleton,
		FbxTransform& transform)
	{
		std::unique_ptr<Bone> bone = std::make_unique<Bone>(
			glm::identity<math::mat4>(),
			glm::identity<math::mat4>(),
			skeleton->GetName());

		*head = &(*head)->AddChild(bonesTree->GetSize(), std::move(bone));
	}

} // namespace erm