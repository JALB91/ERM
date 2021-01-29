#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/loaders/fbx/FBXModelLoader.h"

#include "erm/loaders/fbx/FBXInclude.h"
#include "erm/loaders/fbx/FBXMaterialLoader.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/math/vec.h"

#include "erm/rendering/buffers/IndexData.h"
#include "erm/rendering/buffers/VertexData.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vector>
// clang-format on

namespace erm {

	math::vec3 ToVec3(const FbxVector4& vec)
	{
		return math::vec3(vec.mData[0], vec.mData[1], vec.mData[2]);
	}

	math::vec2 ToVec2(const FbxVector2& vec)
	{
		return math::vec2(vec.mData[0], vec.mData[1]);
	}

	void ProcessNode(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		ResourcesManager& resourcesManager,
		FbxNode* node);

	void ProcessMesh(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		ResourcesManager& resourcesManager,
		FbxMesh* pMesh);

	void ParseFBXModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		ResourcesManager& resourcesManager)
	{
		FbxManager* lSdkManager = FbxManager::Create();

		FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
		auto& t = static_cast<FbxIOBase*>(lImporter)->GetStatus();

		if (!lImporter->Initialize(path, -1, lSdkManager->GetIOSettings()))
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		}

		FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
		lImporter->Import(lScene);
		lImporter->Destroy();

		if (FbxNode* lRootNode = lScene->GetRootNode())
		{
			ProcessNode(mutex, stop, path, model, resourcesManager, lRootNode);
		}

		lSdkManager->Destroy();
	}

	void ProcessNode(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		ResourcesManager& resourcesManager,
		FbxNode* node)
	{
		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			if (stop)
				break;
			FbxNode* child = node->GetChild(i);
			if (FbxMesh* mesh = child->GetMesh())
				ProcessMesh(mutex, stop, path, model, resourcesManager, mesh);

			ProcessNode(mutex, stop, path, model, resourcesManager, child);
		}
	}

	math::vec2 GetUV(FbxMesh* pMesh, int controlPointId, int polygonIndex, int positionInPolygon)
	{
		for (int l = 0; l < pMesh->GetElementUVCount(); ++l)
		{
			FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);

			switch (leUV->GetMappingMode())
			{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
						case FbxGeometryElement::eDirect:
							return ToVec2(leUV->GetDirectArray().GetAt(controlPointId));
						case FbxGeometryElement::eIndexToDirect:
						{
							int id = leUV->GetIndexArray().GetAt(controlPointId);
							return ToVec2(leUV->GetDirectArray().GetAt(id));
						}
						default:
							break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					switch (leUV->GetReferenceMode())
					{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
						{
							int lTextureUVIndex = pMesh->GetTextureUVIndex(polygonIndex, positionInPolygon);
							return ToVec2(leUV->GetDirectArray().GetAt(lTextureUVIndex));
						}
						default:
							break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame: // doesn't make much sense for UVs
				case FbxGeometryElement::eNone: // doesn't make much sense for UVs
					break;
			}
		}

		return math::vec2(0.0f);
	}

	math::vec3 GetNormal(FbxMesh* pMesh, int vertexId)
	{
		for (int l = 0; l < pMesh->GetElementNormalCount(); ++l)
		{
			FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);

			if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				switch (leNormal->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
						return ToVec3(leNormal->GetDirectArray().GetAt(vertexId));
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(vertexId);
						return ToVec3(leNormal->GetDirectArray().GetAt(id));
					}
					default:
						break; // other reference modes not shown here!
				}
			}
		}

		return math::vec3(0.0f);
	}

	void ProcessMesh(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		ResourcesManager& resourcesManager,
		FbxMesh* pMesh)
	{
		const int lPolygonCount = pMesh->GetPolygonCount();
		const FbxVector4* lControlPoints = pMesh->GetControlPoints();

		std::vector<VertexData> vData;
		std::vector<IndexData> iData;

		for (int i = 0; i < lPolygonCount; ++i)
		{
			const int lPolygonSize = pMesh->GetPolygonSize(i);

			if (lPolygonSize < 3 || lPolygonSize > 4)
				continue;

			iData.emplace_back(static_cast<int>(vData.size()));
			iData.emplace_back(static_cast<int>(vData.size() + 1));
			iData.emplace_back(static_cast<int>(vData.size() + 2));

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

			if (lPolygonSize == 4)
			{
				int lControlPointIndex3 = pMesh->GetPolygonVertex(i, 3);
				VertexData data3;
				data3.mPositionVertex = ToVec3(lControlPoints[lControlPointIndex3]);
				data3.mNormalVertex = GetNormal(pMesh, static_cast<int>(vData.size() + 3));
				data3.mUVVertex = GetUV(pMesh, lControlPointIndex3, i, 3);

				iData.emplace_back(static_cast<uint32_t>(vData.size() + 2));
				iData.emplace_back(static_cast<uint32_t>(vData.size() + 3));
				iData.emplace_back(static_cast<uint32_t>(vData.size()));

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
			mat = ParseFBXMaterial(mutex, stop, path, pMesh, resourcesManager);
		}

		unsigned int vCount = static_cast<unsigned int>(vData.size());
		VertexData* verticesData = new VertexData[vCount];
		for (unsigned int i = 0; i < vCount; ++i)
			verticesData[i] = vData[i];

		unsigned int iCount = static_cast<unsigned int>(iData.size());
		IndexData* indicesData = new IndexData[iCount];
		for (unsigned int i = 0; i < iCount; ++i)
			indicesData[i] = iData[i];

		RenderConfigs conf = RenderConfigs::MODELS_RENDER_CONFIGS;
		conf.mMaterial = mat;
		mutex.lock();
		model.AddMesh(verticesData, static_cast<uint32_t>(vData.size()), indicesData, static_cast<uint32_t>(iData.size()), conf);
		mutex.unlock();
	}

} // namespace erm

#endif