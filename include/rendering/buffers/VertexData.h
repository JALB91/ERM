#pragma once

#include "math/vec.h"

#include <glm/glm.hpp>

namespace erm {
	
	static const unsigned int kPositionVectorsLenght = 3;
	static const unsigned int kNormalVectorsLenght = 3;
	static const unsigned int kUVVectorsLenght = 2;
	static const unsigned int kBoneIdsCount = 3;
	static const unsigned int kBoneWeightsCount = 3;
	
	typedef float VertexType;
	
	typedef math::vec<kPositionVectorsLenght, VertexType> PositionVertex;
	typedef math::vec<kNormalVectorsLenght, VertexType> NormalVertex;
	typedef math::vec<kUVVectorsLenght, VertexType> UVVertex;
	typedef math::vec<kBoneIdsCount, unsigned int> BoneIds;
	typedef math::vec<kBoneWeightsCount, VertexType> BoneWeights;
	
	struct VertexData
	{
		PositionVertex mPositionVertex;
		NormalVertex mNormalVertex;
		UVVertex mUVVertex;
		BoneIds mBoneIds;
		BoneWeights mBoneWeights;
	};
	
}
