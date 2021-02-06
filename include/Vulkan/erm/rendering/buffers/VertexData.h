#pragma once

#include "erm/math/vec.h"

#include <glm/glm.hpp>

namespace erm {

	static const unsigned int kPositionVectorsLenght = 3;
	static const unsigned int kNormalVectorsLenght = 3;
	static const unsigned int kUVVectorsLenght = 2;
	static const unsigned int kMaxBonesNumber = 4;

	typedef float VertexType;
	typedef int IdType;

	typedef math::vec<kPositionVectorsLenght, VertexType> PositionVertex;
	typedef math::vec<kNormalVectorsLenght, VertexType> NormalVertex;
	typedef math::vec<kUVVectorsLenght, VertexType> UVVertex;
	typedef math::vec<kMaxBonesNumber, VertexType> BoneWeights;
	typedef math::vec<kMaxBonesNumber, IdType> BoneIds;
	typedef int BoneNum;

	struct VertexData
	{
		PositionVertex mPositionVertex = PositionVertex(0.0f);
		NormalVertex mNormalVertex = NormalVertex(0.0f);
		UVVertex mUVVertex = UVVertex(0.0f);
		BoneWeights mBoneWeights = BoneWeights(0.0f);
		BoneIds mBoneIds = BoneIds(0);
		BoneNum mBoneNum = BoneNum(0);
	};

} // namespace erm
