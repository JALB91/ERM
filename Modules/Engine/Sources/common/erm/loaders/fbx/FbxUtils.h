#pragma once

#include "erm/loaders/fbx/FBXInclude.h"
#include "erm/loaders/fbx/FBXSkeletonData.h"

#include "erm/math/mat.h"
#include "erm/math/quat.h"
#include "erm/math/vec.h"

#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/VertexData.h"

#include <map>
#include <vector>

namespace erm {

extern math::mat4 ToMat4(const FbxMatrix& matrix);
extern math::mat4 ToMat4(const FbxAMatrix& matrix);
extern math::quat ToQuat(const FbxQuaternion& quat);
extern math::vec4 ToVec4(const FbxVector4& vec);
extern math::vec3 ToVec3(const FbxVector4& vec);
extern math::vec2 ToVec2(const FbxVector2& vec);
extern math::vec3 GetNormal(FbxMesh* pMesh, int vertexId);
extern math::vec2 GetUV(FbxMesh* pMesh, int controlPointId, int polygonIndex, int positionInPolygon);
extern void GetBonesData(
	const std::map<int, std::vector<FbxSkeletonData>>& skeletonData,
	const BonesTree& bonesTree,
	VertexData& data,
	int controlPointIndex);

} // namespace erm
