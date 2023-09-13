#include "erm/resources/loaders/fbx/FBXSkeletonLoader.h"
#include "erm/resources/loaders/fbx/FbxUtils.h"
#include "erm/resources/data_structs/Bone.h"

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
		ProcessNode(mutex, stop, bonesTree, nullptr, glm::identity<math::mat4>(), node);
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
		if (boneName.empty())
			boneName = node->GetName();

		const math::mat4 bindMatrix = ToMat4(node->EvaluateLocalTransform());
		parentBind *= bindMatrix;
		const math::mat4 inverseBind = glm::inverse(parentBind);

		if (!head)
		{
			if (!bonesTree)
			{
				bonesTree = std::make_unique<BonesTree>(0, Bone(bindMatrix, inverseBind, boneName.c_str()));
				head = bonesTree.get();
			}
			else
			{
				auto newRoot = std::make_unique<BonesTree>(0, Bone(glm::identity<math::mat4>(), glm::identity<math::mat4>(), "ReaaaalRoot"));
				bonesTree->ForEachDo([&bonesTree](BonesTree& node) {
					node.SetId(node.GetId() + static_cast<BoneID>(bonesTree->GetChildren().size()) + 1);
				});
				newRoot->AddChild(std::move(bonesTree));
				
				bonesTree = std::move(newRoot);
				head = bonesTree.get();
				head->AddChild(1, Bone(bindMatrix, inverseBind, boneName.c_str()));
			}
		}
		else if (head)
		{
			head = &head->AddChild(head->GetRoot().GetSize(), Bone(bindMatrix, inverseBind, boneName.c_str()));
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
