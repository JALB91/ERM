#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/loaders/fbx/FBXSkeletonLoader.h"
#include "erm/loaders/fbx/FbxUtils.h"
// clang-format on

namespace erm {

	void ProcessNode(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		math::mat4 parentBind,
		FbxNode* node);

	void ProcessBone(
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		math::mat4& parentBind,
		FbxSkeleton* bone);

	void ProcessSkeleton(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		FbxScene& scene)
	{
		BonesTree* head = bonesTree.get();
		if (FbxNode* node = scene.GetRootNode())
			ProcessNode(mutex, stop, bonesTree, &head, glm::identity<math::mat4>(), node);
	}

	void ProcessNode(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		math::mat4 parentBind,
		FbxNode* node)
	{
		BonesTree* tmpHead = *head;

		if (FbxSkeleton* bone = node->GetSkeleton())
			ProcessBone(bonesTree, head, parentBind, bone);

		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			if (stop)
				break;

			ProcessNode(mutex, stop, bonesTree, head, parentBind, node->GetChild(i));
		}

		*head = tmpHead ? tmpHead : bonesTree.get();
	}

	void ProcessBone(
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree** head,
		math::mat4& parentBind,
		FbxSkeleton* bone)
	{
		math::mat4 bindMatrix = ToMat4(bone->GetNode()->EvaluateLocalTransform());
		parentBind *= bindMatrix;
		math::mat4 inverseBind = glm::inverse(parentBind);

		if (!bonesTree)
		{
			bonesTree = std::make_unique<BonesTree>(0, std::make_unique<Bone>(bindMatrix, inverseBind, bone->GetName()));
			*head = &bonesTree->GetRoot();
		}
		else
		{
			*head = &(*head)->AddChild(bonesTree->GetSize(), std::make_unique<Bone>(bindMatrix, inverseBind, bone->GetName()));
		}
	}

} // namespace erm

#endif