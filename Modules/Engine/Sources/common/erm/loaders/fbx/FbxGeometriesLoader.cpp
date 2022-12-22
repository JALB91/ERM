// clang-format off
#include "erm/loaders/fbx/FbxGeometriesLoader.h"

#include "erm/loaders/fbx/FBXMaterialLoader.h"
#include "erm/loaders/fbx/FbxUtils.h"
#include "erm/loaders/fbx/FBXSkeletonData.h"

#include "erm/math/math.h"

#include "erm/rendering/animations/SkeletonAnimation.h"
#include "erm/rendering/data_structs/VertexData.h"
#include "erm/rendering/data_structs/IndexData.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/materials/Material.h"

#include <map>
#include <vector>
// clang-format on

namespace erm {

void ProcessNode(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	uint32_t& indicesOffset,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxNode* node,
	std::vector<std::unique_ptr<SkeletonAnimation>>& animations);

void ProcessMesh(
	std::mutex& mutex,
	const char* path,
	Model& model,
	uint32_t& indicesOffset,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxMesh* pMesh,
	std::vector<std::unique_ptr<SkeletonAnimation>>& animations);

void ProcessGeometries(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxScene& scene,
	std::vector<std::unique_ptr<SkeletonAnimation>>& animations)
{
	uint32_t indicesOffset = 0;
	if (FbxNode* node = scene.GetRootNode())
		ProcessNode(mutex, stop, path, model, indicesOffset, resourcesManager, bonesTree, node, animations);
}

void ProcessNode(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	uint32_t& indicesOffset,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxNode* node,
	std::vector<std::unique_ptr<SkeletonAnimation>>& animations)
{
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		if (stop)
			break;

		FbxNode* child = node->GetChild(i);
		if (FbxMesh* mesh = child->GetMesh())
			ProcessMesh(mutex, path, model, indicesOffset, resourcesManager, bonesTree, mesh, animations);

		ProcessNode(mutex, stop, path, model, indicesOffset, resourcesManager, bonesTree, child, animations);
	}
}

void ProcessMesh(
	std::mutex& mutex,
	const char* path,
	Model& model,
	uint32_t& indicesOffset,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxMesh* pMesh,
	std::vector<std::unique_ptr<SkeletonAnimation>>& animations)
{
	if (pMesh->RemoveBadPolygons() < 0)
		return;

	FbxGeometryConverter converter(pMesh->GetFbxManager());
	pMesh = static_cast<FbxMesh*>(converter.Triangulate(pMesh, true));

	if (!pMesh || pMesh->RemoveBadPolygons() < 0)
		return;

	std::map<int, std::vector<FbxSkeletonData>> skeletonDataMap;
	const int deformerCount = pMesh->GetDeformerCount();
	if (deformerCount > 0 && bonesTree)
	{
		for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
		{
			FbxDeformer* deformer = pMesh->GetDeformer(deformerIndex);

			if (!deformer || deformer->GetDeformerType() != FbxDeformer::eSkin)
				continue;

			FbxSkin* skin = (FbxSkin*)deformer;

			if (!skin)
				continue;

			const int clusterCount = skin->GetClusterCount();

			for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);

				if (!cluster)
					continue;

				FbxNode* linkNode = cluster->GetLink();

				if (!linkNode)
					continue;

				std::string targetName = cluster->GetName();
				if (targetName.empty())
					targetName = linkNode->GetName();

				const int associatedCtrlPointCount = cluster->GetControlPointIndicesCount();
				const int* pCtrlPointIndices = cluster->GetControlPointIndices();
				const double* pCtrlPointWeights = cluster->GetControlPointWeights();

				for (int ctrlPointIndex = 0; ctrlPointIndex < associatedCtrlPointCount; ++ctrlPointIndex)
				{
					const int targetIndex = pCtrlPointIndices[ctrlPointIndex];
					const double targetWeight = pCtrlPointWeights[ctrlPointIndex];

					if (skeletonDataMap[targetIndex].size() >= BoneIds::length() || targetWeight <= 0.0)
						continue;

					skeletonDataMap[targetIndex].emplace_back(
						targetName.c_str(),
						static_cast<float>(targetWeight));
				}

				BonesTree* targetNode = nullptr;
				bonesTree->ForEachDo([&targetNode, &targetName](BonesTree& node) {
					if (!targetNode && node.GetPayload().mName == targetName)
						targetNode = &node;
				});

				if (!targetNode || targetNode->GetId() >= MAX_BONES)
					continue;

				FbxAMatrix transformMatrix;
				FbxAMatrix transformLinkMatrix;
				FbxAMatrix globalBindposeInverseMatrix;
				cluster->GetTransformMatrix(transformMatrix);

				// The transformation of the mesh at binding time
				cluster->GetTransformLinkMatrix(transformLinkMatrix);

				// The transformation of the cluster(joint) at binding time from joint space to world space
				globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix;

				targetNode->GetPayload().mInverseBindTransform = ToMat4(globalBindposeInverseMatrix);

				static const fbxsdk::FbxTime::EMode targetTimeMode = fbxsdk::FbxTime::EMode::eFrames24;
				static const uint32_t targetFps = 24;
				static const float scale = 1.0f;

				FbxScene* scene = pMesh->GetScene();

				for (int animIndex = 0; animIndex < scene->GetSrcObjectCount<FbxAnimStack>(); ++animIndex)
				{
					FbxAnimStack* currAnimStack = scene->GetSrcObject<FbxAnimStack>(animIndex);

					scene->SetCurrentAnimationStack(currAnimStack);

					FbxString animStackName = currAnimStack->GetName();
					std::string animationName = animStackName.Buffer();
					FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
					fbxsdk::FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
					fbxsdk::FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
					auto animationLength = (end.GetFrameCount(targetTimeMode) - start.GetFrameCount(targetTimeMode));

					auto it = std::find_if(animations.begin(), animations.end(), [&animationName](std::unique_ptr<SkeletonAnimation>& animation) {
						return Utils::CompareNoCaseSensitive(animation->mName, animationName);
					});

					SkeletonAnimation* animation = nullptr;

					if (it == animations.end())
					{
						animation = animations.emplace_back(std::make_unique<SkeletonAnimation>()).get();
						animation->mName = animationName;
						animation->mTotalAnimationTime = (static_cast<float>(animationLength) / static_cast<float>(targetFps)) * scale;
					}
					else
					{
						animation = it->get();
					}

					/*
					* TODO: Find a way to parse animation data from the layers rather than using EvaluateLocalTransform
					const int nbAnimLayers = currAnimStack->GetMemberCount<FbxAnimLayer>();
					for (int animLayerIndex = 0; animLayerIndex < nbAnimLayers; ++animLayerIndex)
					{
						FbxAnimLayer* animLayer = currAnimStack->GetMember<FbxAnimLayer>(animLayerIndex);
						static const std::array targetComponents = {FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z};
						static const auto updateValue = [&](auto& prop, const char* component, int offset, auto updateFunc) {
							FbxAnimCurve* animCurve = prop.GetCurve(animLayer, component);
							if (animCurve)
							{
								const int keyCount = animCurve->KeyGetCount();

								for (int keyIndex = 0; keyIndex < keyCount; ++keyIndex)
								{
									float keyValue = animCurve->KeyGetValue(keyIndex);
									FbxTime keyTime = animCurve->KeyGetTime(keyIndex);

									const float targetTS = static_cast<float>(keyTime.GetSecondDouble());

									auto it = std::find_if(animation->mKeyFrames.begin(), animation->mKeyFrames.end(), [targetTS](const KeyFrame& keyFrame) {
										return std::fabs(keyFrame.mTimestamp - targetTS) < std::numeric_limits<float>().epsilon();
									});
									
									KeyFrame* targetKeyFrame = nullptr;

									if (it == animation->mKeyFrames.end())
									{
										targetKeyFrame = &animation->mKeyFrames.emplace_back(targetTS);
									}
									else
									{
										targetKeyFrame = &(*it);
									}

									updateFunc(targetKeyFrame->mTransforms[targetNode->GetId()], keyValue, offset);
								}
							}
						};

						static const auto updateTranslationValue = [](Pose& pose, float keyValue, int offset) {
							pose.mTranslation[offset] = keyValue;
						};

						static const auto updateScaleValue = [](Pose& pose, float keyValue, int offset) {
							pose.mScale[offset] = keyValue;
						};

						static const auto updateRotationValue = [](Pose& pose, float keyValue, int offset) {
							math::quat& quat = pose.mRotation;

							switch (offset)
							{
								case 0:
									quat = glm::rotate(quat, glm::radians(keyValue), math::vec3(1.0f, 0.0f, 0.0f));
									break;
								case 1:
									quat = glm::rotate(quat, glm::radians(keyValue), math::vec3(0.0f, 1.0f, 0.0f));
									break;
								case 2:
									quat = glm::rotate(quat, glm::radians(keyValue), math::vec3(0.0f, 0.0f, 1.0f));
									break;
							}
						};

						for (int componentIndex = 0; componentIndex < targetComponents.size(); ++componentIndex)
						{
							updateValue(linkNode->LclTranslation, targetComponents[componentIndex], componentIndex, updateTranslationValue);
							updateValue(linkNode->LclScaling, targetComponents[componentIndex], componentIndex, updateScaleValue);
							updateValue(linkNode->LclRotation, targetComponents[componentIndex], componentIndex, updateRotationValue);
						}	
					}*/

					int curr = 0;
					for (FbxLongLong i = start.GetFrameCount(targetTimeMode); i <= end.GetFrameCount(targetTimeMode); ++i)
					{
						fbxsdk::FbxTime currTime;
						currTime.SetFrame(i, targetTimeMode);
						const float targetMs = (static_cast<float>(curr++) / static_cast<float>(targetFps)) * scale;
						auto it = std::find_if(animation->mKeyFrames.begin(), animation->mKeyFrames.end(), [targetMs](const KeyFrame& keyFrame) {
							return std::fabs(keyFrame.mTimestamp - targetMs) < std::numeric_limits<float>().epsilon();
						});
						KeyFrame* targetKeyFrame = nullptr;
						if (it == animation->mKeyFrames.end())
						{
							targetKeyFrame = &animation->mKeyFrames.emplace_back(targetMs);
						}
						else
						{
							targetKeyFrame = &(*it);
						}

						FbxAMatrix currentTransformOffset = linkNode->EvaluateLocalTransform(currTime);
						
						math::DecomposeMatrix(
							ToMat4(currentTransformOffset), 
							targetKeyFrame->mTransforms[targetNode->GetId()].mTranslation, 
							targetKeyFrame->mTransforms[targetNode->GetId()].mRotation, 
							targetKeyFrame->mTransforms[targetNode->GetId()].mScale);
					}
				}
			}
		}
	}

	const int lPolygonCount = pMesh->GetPolygonCount();
	const FbxVector4* lControlPoints = pMesh->GetControlPoints();

	std::vector<VertexData> vData;
	std::vector<IndexData> iData;

	for (int i = 0; i < lPolygonCount; ++i)
	{
		ERM_ASSERT(pMesh->GetPolygonSize(i) == 3);

		iData.emplace_back(static_cast<int>(indicesOffset + vData.size()));
		iData.emplace_back(static_cast<int>(indicesOffset + vData.size() + 1));
		iData.emplace_back(static_cast<int>(indicesOffset + vData.size() + 2));

		int lControlPointIndex0 = pMesh->GetPolygonVertex(i, 0);
		VertexData data0;
		data0.mPositionVertex = ToVec3(lControlPoints[lControlPointIndex0]);
		data0.mNormalVertex = GetNormal(pMesh, static_cast<int>(vData.size()));
		data0.mUVVertex = GetUV(pMesh, lControlPointIndex0, i, 0);
		
		int lControlPointIndex1 = pMesh->GetPolygonVertex(i, 1);
		VertexData data1;
		data1.mPositionVertex = ToVec3(lControlPoints[lControlPointIndex1]);
		data1.mNormalVertex = GetNormal(pMesh, static_cast<int>(vData.size() + 1));
		data1.mUVVertex = GetUV(pMesh, lControlPointIndex1, i, 1);
		
		int lControlPointIndex2 = pMesh->GetPolygonVertex(i, 2);
		VertexData data2;
		data2.mPositionVertex = ToVec3(lControlPoints[lControlPointIndex2]);
		data2.mNormalVertex = GetNormal(pMesh, static_cast<int>(vData.size() + 2));
		data2.mUVVertex = GetUV(pMesh, lControlPointIndex2, i, 2);
		
		if (bonesTree)
		{
			GetBonesData(skeletonDataMap, *bonesTree, data0, lControlPointIndex0);
			GetBonesData(skeletonDataMap, *bonesTree, data1, lControlPointIndex1);
			GetBonesData(skeletonDataMap, *bonesTree, data2, lControlPointIndex2);
		}

		vData.emplace_back(data0);
		vData.emplace_back(data1);
		vData.emplace_back(data2);
	}

	if (vData.empty() || iData.empty())
		return;

	Material* mat = nullptr;
	if (pMesh->GetElementMaterialCount() > 0)
	{
		mat = ParseFBXMaterial(mutex, path, pMesh, resourcesManager);
	}

	PipelineConfigs conf = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS;
	conf.mMaterial.mType = MaterialType::LEGACY;
	conf.mMaterial.mData = mat ? mat : &Material::DEFAULT;

	indicesOffset += static_cast<uint32_t>(vData.size());

	mutex.lock();
	model.AddMesh(std::move(vData), std::move(iData), RenderConfigs::DEFAULT_RENDER_CONFIGS, conf, pMesh->GetName());
	mutex.unlock();
}

} // namespace erm
