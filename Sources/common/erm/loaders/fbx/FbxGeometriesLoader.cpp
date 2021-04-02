#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/loaders/fbx/FbxGeometriesLoader.h"

#include "erm/loaders/fbx/FBXMaterialLoader.h"
#include "erm/loaders/fbx/FbxUtils.h"
#include "erm/loaders/fbx/FBXSkeletonData.h"

#include "erm/rendering/data_structs/VertexData.h"
#include "erm/rendering/data_structs/IndexData.h"
#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/data_structs/Model.h"

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
	FbxNode* node);

void ProcessMesh(
	std::mutex& mutex,
	const char* path,
	Model& model,
	uint32_t& indicesOffset,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxMesh* pMesh);

void ProcessGeometries(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxScene& scene)
{
	uint32_t indicesOffset = 0;
	if (FbxNode* node = scene.GetRootNode())
		ProcessNode(mutex, stop, path, model, indicesOffset, resourcesManager, bonesTree, node);
}

void ProcessNode(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	uint32_t& indicesOffset,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxNode* node)
{
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		if (stop)
			break;

		FbxNode* child = node->GetChild(i);
		if (FbxMesh* mesh = child->GetMesh())
			ProcessMesh(mutex, path, model, indicesOffset, resourcesManager, bonesTree, mesh);

		ProcessNode(mutex, stop, path, model, indicesOffset, resourcesManager, bonesTree, child);
	}
}

void ProcessMesh(
	std::mutex& mutex,
	const char* path,
	Model& model,
	uint32_t& indicesOffset,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxMesh* pMesh)
{
	std::map<int, std::vector<FbxSkeletonData>> skeletonData;
	const int deformerCount = pMesh->GetDeformerCount();
	if (deformerCount > 0)
	{
		for (int i = 0; i < deformerCount; ++i)
		{
			FbxDeformer* deformer = pMesh->GetDeformer(i);

			if (!deformer || deformer->GetDeformerType() != FbxDeformer::eSkin)
				continue;

			FbxSkin* skin = (FbxSkin*)deformer;

			if (!skin)
				continue;

			const int clusterCount = skin->GetClusterCount();

			for (int j = 0; j < clusterCount; ++j)
			{
				FbxCluster* cluster = skin->GetCluster(j);

				if (!cluster)
					continue;

				FbxNode* linkNode = cluster->GetLink();

				if (!linkNode)
					continue;

				const int associatedCtrlPointCount = cluster->GetControlPointIndicesCount();
				const int* pCtrlPointIndices = cluster->GetControlPointIndices();
				const double* pCtrlPointWeights = cluster->GetControlPointWeights();

				for (int k = 0; k < associatedCtrlPointCount; ++k)
				{
					if (skeletonData[pCtrlPointIndices[k]].size() >= BoneIds::length() || pCtrlPointWeights[k] <= 0.0f)
						continue;
					skeletonData[pCtrlPointIndices[k]].emplace_back(
						cluster->GetName(),
						static_cast<float>(pCtrlPointWeights[k]));
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
		const int lPolygonSize = pMesh->GetPolygonSize(i);

		if (lPolygonSize < 3 || lPolygonSize > 4)
			continue;

		iData.emplace_back(static_cast<int>(indicesOffset + vData.size()));
		iData.emplace_back(static_cast<int>(indicesOffset + vData.size() + 1));
		iData.emplace_back(static_cast<int>(indicesOffset + vData.size() + 2));

		int lControlPointIndex0 = pMesh->GetPolygonVertex(i, 0);
		VertexData data0;
		data0.mPositionVertex = ToVec3(lControlPoints[lControlPointIndex0]);
		data0.mNormalVertex = GetNormal(pMesh, static_cast<int>(vData.size()));
		data0.mUVVertex = GetUV(pMesh, lControlPointIndex0, i, 0);
		GetBonesData(skeletonData, bonesTree.get(), data0, lControlPointIndex0);

		int lControlPointIndex1 = pMesh->GetPolygonVertex(i, 1);
		VertexData data1;
		data1.mPositionVertex = ToVec3(lControlPoints[lControlPointIndex1]);
		data1.mNormalVertex = GetNormal(pMesh, static_cast<int>(vData.size() + 1));
		data1.mUVVertex = GetUV(pMesh, lControlPointIndex1, i, 1);
		GetBonesData(skeletonData, bonesTree.get(), data1, lControlPointIndex1);

		int lControlPointIndex2 = pMesh->GetPolygonVertex(i, 2);
		VertexData data2;
		data2.mPositionVertex = ToVec3(lControlPoints[lControlPointIndex2]);
		data2.mNormalVertex = GetNormal(pMesh, static_cast<int>(vData.size() + 2));
		data2.mUVVertex = GetUV(pMesh, lControlPointIndex2, i, 2);
		GetBonesData(skeletonData, bonesTree.get(), data2, lControlPointIndex2);

		if (lPolygonSize == 4)
		{
			int lControlPointIndex3 = pMesh->GetPolygonVertex(i, 3);
			VertexData data3;
			data3.mPositionVertex = ToVec3(lControlPoints[lControlPointIndex3]);
			data3.mNormalVertex = GetNormal(pMesh, static_cast<int>(vData.size() + 3));
			data3.mUVVertex = GetUV(pMesh, lControlPointIndex3, i, 3);
			GetBonesData(skeletonData, bonesTree.get(), data3, lControlPointIndex3);

			iData.emplace_back(indicesOffset + static_cast<uint32_t>(vData.size() + 2));
			iData.emplace_back(indicesOffset + static_cast<uint32_t>(vData.size() + 3));
			iData.emplace_back(indicesOffset + static_cast<uint32_t>(vData.size()));

			vData.emplace_back(data0);
			vData.emplace_back(data1);
			vData.emplace_back(data2);
			vData.emplace_back(data3);
		}
		else
		{
			vData.emplace_back(data0);
			vData.emplace_back(data1);
			vData.emplace_back(data2);
		}
	}

	Material* mat = nullptr;
	if (pMesh->GetElementMaterialCount() > 0)
	{
		mat = ParseFBXMaterial(mutex, path, pMesh, resourcesManager);
	}

	RenderConfigs conf = RenderConfigs::MODELS_RENDER_CONFIGS;
	conf.mMaterial.mType = MaterialType::LEGACY;
	conf.mMaterial.mData = mat;

	indicesOffset += static_cast<uint32_t>(vData.size());

	mutex.lock();
	model.AddMesh(std::move(vData), std::move(iData), conf, pMesh->GetName());
	mutex.unlock();
}

} // namespace erm

#endif
