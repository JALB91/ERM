#pragma once

#include "erm/math/vec.h"

#include <glm/glm.hpp>

namespace erm {

typedef float VertexType;
typedef int IdType;

typedef math::vec<3, VertexType> PositionVertex;
typedef math::vec<3, VertexType> NormalVertex;
typedef math::vec<2, VertexType> UVVertex;
typedef math::vec<4, VertexType> BoneWeights;
typedef math::vec<4, IdType> BoneIds;
typedef int BoneNum;
typedef int DebugBoneId;

struct VertexData
{
	PositionVertex mPositionVertex = PositionVertex(0.0f);
	NormalVertex mNormalVertex = NormalVertex(0.0f);
	UVVertex mUVVertex = UVVertex(0.0f);
	BoneWeights mBoneWeights = BoneWeights(0.0f);
	BoneIds mBoneIds = BoneIds(0);
	BoneNum mBoneNum = BoneNum(0);
	DebugBoneId mDebugBoneId = DebugBoneId(0);
};

} // namespace erm
