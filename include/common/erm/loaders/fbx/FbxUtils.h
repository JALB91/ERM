#pragma once

#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/loaders/fbx/FBXInclude.h"

#include "erm/math/mat.h"
#include "erm/math/vec.h"
// clang-format on

namespace erm {

	extern math::mat4 ToMat4(const FbxAMatrix& matrix);
	extern math::vec3 ToVec3(const FbxVector4& vec);
	extern math::vec2 ToVec2(const FbxVector2& vec);
	extern math::vec3 GetNormal(FbxMesh* pMesh, int vertexId);
	extern math::vec2 GetUV(FbxMesh* pMesh, int controlPointId, int polygonIndex, int positionInPolygon);

} // namespace erm

#endif