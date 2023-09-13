#include "erm/resources/loaders/fbx/FbxUtils.h"
#include "erm/resources/data_structs/Bone.h"

namespace erm {

math::mat4 ToMat4(const FbxMatrix& matrix)
{
	math::mat4 result;
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			result[y][x] = static_cast<float>(matrix.Get(y, x));
	return result;
}

math::mat4 ToMat4(const FbxAMatrix& matrix)
{
	math::mat4 result;
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			result[y][x] = static_cast<float>(matrix.Get(y, x));
	return result;
}

math::quat ToQuat(const FbxQuaternion& quat)
{
	math::quat result = glm::identity<math::quat>();
	result[0] = static_cast<float>(quat[0]);
	result[1] = static_cast<float>(quat[1]);
	result[2] = static_cast<float>(quat[2]);
	result[3] = static_cast<float>(quat[3]);
	return result;
}

math::vec4 ToVec4(const FbxVector4& vec)
{
	return math::vec4(vec.mData[0], vec.mData[1], vec.mData[2], vec.mData[3]);
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
			{
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
			}
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
				break;
			}
		}
	}

	return math::vec2(0.0f);
}

void GetBonesData(
	const std::map<int, std::vector<FbxSkeletonData>>& skeletonDataMap,
	const BonesTree& bonesTree,
	VertexData& vertexData,
	int controlPointIndex)
{
	auto it = skeletonDataMap.find(controlPointIndex);
	if (it == skeletonDataMap.end())
		return;

	auto& skeletonDataVec = it->second;

	for (auto& skeletonData : skeletonDataVec)
	{
		if (vertexData.mBoneNum >= BoneIds::length())
			break;

		if (skeletonData.mBoneWeight <= 0.0f)
			continue;

		const BonesTree* node = bonesTree.FindByPayload([&skeletonData](const Bone& bone) {
			return skeletonData.mBoneID == bone.mName;
		});

		if (!node)
			continue;

		const BoneID id = node->GetId();

		bool skip = false;
		for (int i = 0; i < BoneIds::length(); ++i)
		{
			if (vertexData.mBoneIds[i] == static_cast<int>(id) && vertexData.mBoneWeights[i] > 0.0f)
			{
				skip = true;
				break;
			}
		}

		if (skip)
			continue;

		vertexData.mBoneIds[vertexData.mBoneNum] = id;
		vertexData.mBoneWeights[vertexData.mBoneNum] = skeletonData.mBoneWeight;
		vertexData.mBoneNum++;
	}
}

} // namespace erm
