#pragma once

#include <erm/math/vec.h>

#include <erm/assets/models/VertexTypes.h>

#include <glm/glm.hpp>

namespace erm {

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
