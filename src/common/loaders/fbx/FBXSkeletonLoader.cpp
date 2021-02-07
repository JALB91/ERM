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
		BonesTree* head,
		math::mat4 parentBind,
		FbxNode* node);

	void ProcessSkeleton(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		FbxScene& scene)
	{
		if (FbxNode* node = scene.GetRootNode())
			ProcessNode(mutex, stop, bonesTree, bonesTree.get(), glm::identity<math::mat4>(), node);
	}

	void ProcessNode(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		std::unique_ptr<BonesTree>& bonesTree,
		BonesTree* head,
		math::mat4 parentBind,
		FbxNode* node)
	{
		if (FbxSkeleton* bone = node->GetSkeleton())
		{
			std::string boneName = bone->GetName();

			if (!Utils::EndsWith(boneName, "_end"))
			{
				math::mat4 bindMatrix = ToMat4(node->EvaluateLocalTransform());
				parentBind *= bindMatrix;
				const math::mat4 inverseBind = glm::inverse(parentBind);

				if (!bonesTree)
				{
					bonesTree = std::make_unique<BonesTree>(0, std::make_unique<Bone>(bindMatrix, inverseBind, boneName.c_str()));
					head = bonesTree.get();
				}
				else
				{
					head = &head->AddChild(bonesTree->GetSize(), std::make_unique<Bone>(bindMatrix, inverseBind, boneName.c_str()));
				}
			}
		}

		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			if (stop)
				break;

			ProcessNode(mutex, stop, bonesTree, head, parentBind, node->GetChild(i));
		}
	}

} // namespace erm

#endif