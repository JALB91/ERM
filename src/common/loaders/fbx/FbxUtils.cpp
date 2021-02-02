#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/loaders/fbx/FbxUtils.h"
// clang-format on

namespace erm {

	math::mat4 ToMat4(const FbxAMatrix& matrix)
	{
		math::mat4 result;
		for (int x = 0; x < 4; ++x)
			for (int y = 0; y < 4; ++y)
				result[y][x] = static_cast<float>(matrix.Get(y, x));
		return result;
	}

	math::vec3 ToVec3(const FbxVector4& vec)
	{
		return math::vec3(vec.mData[0], vec.mData[1], vec.mData[2]);
	}

	math::vec2 ToVec2(const FbxVector2& vec)
	{
		return math::vec2(vec.mData[0], vec.mData[1]);
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

	void GetBonesData(
		const std::map<int, std::vector<FbxSkeletonData>>& skeletonData,
		BonesTree* bonesTree,
		VertexData& data,
		int controlPointIndex)
	{
		auto it = skeletonData.find(controlPointIndex);
		if (it != skeletonData.end())
		{
			auto& vec = it->second;

			for (auto& val : vec)
			{
				if (data.mBoneNum >= kMaxBonesNumber)
					break;

				if (val.mBoneWeight <= 0.0f)
					continue;

				BonesTree* bone = nullptr;
				bonesTree->ForEachDo([&bone, &val](BonesTree& node) {
					if (!bone && val.mBoneName == node.GetPayload()->mName)
						bone = &node;
				});

				if (!bone)
					continue;

				const auto id = bone->GetId();

				bool skip = false;
				for (int i = 0; i < kMaxBonesNumber; ++i)
				{
					if (data.mBoneIds[i] == id && data.mBoneWeights[i] > 0.0f)
					{
						skip = true;
						break;
					}
				}

				if (skip)
					continue;

				data.mBoneIds[data.mBoneNum] = id;
				data.mBoneWeights[data.mBoneNum] = val.mBoneWeight;
				data.mBoneNum++;
			}
		}
	}

} // namespace erm

#endif