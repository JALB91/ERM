#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/loaders/fbx/FBXSkeletonLoader.h"
// clang-format on

namespace erm {

	void ProcessNode(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		FbxNode* node);

	void ProcessBone(
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		FbxSkeleton* bone);

	void ProcessSkeleton(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		FbxScene& scene)
	{
		BonesTree* head = bonesTree.get();
		if (FbxNode* node = scene.GetRootNode())
			ProcessNode(mutex, stop, bonesTree, &head, node);
	}

	void ProcessNode(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		FbxNode* node)
	{
		BonesTree* tmpHead = *head;
		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			if (stop)
				break;

			FbxNode* child = node->GetChild(i);
			if (FbxSkeleton* bone = child->GetSkeleton())
				ProcessBone(bonesTree, head, bone);

			ProcessNode(mutex, stop, bonesTree, head, child);

			*head = tmpHead ? tmpHead : bonesTree.get();
		}
	}

	void ProcessBone(
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		FbxSkeleton* bone)
	{
		if (!bonesTree)
		{
			bonesTree = std::make_unique<BonesTree>(0, std::make_unique<Bone>(glm::identity<math::mat4>(), glm::identity<math::mat4>(), bone->GetName()));
			*head = &bonesTree->GetRoot();
		}
		else
		{
			*head = &(*head)->AddChild(bonesTree->GetSize(), std::make_unique<Bone>(glm::identity<math::mat4>(), glm::identity<math::mat4>(), bone->GetName()));
		}
	}

} // namespace erm

#endif