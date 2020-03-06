#pragma once

#include "erm/math/vec.h"

#include <glm/glm.hpp>

namespace erm {
	
	static const unsigned int kPositionVectorsLenght = 3;
	static const unsigned int kNormalVectorsLenght = 3;
	static const unsigned int kUVVectorsLenght = 2;
	static const unsigned int kMaxBonesNumber = 4;
	
	typedef float VertexType;
	typedef unsigned int IdType;
	
	typedef math::vec<kPositionVectorsLenght, VertexType> PositionVertex;
	typedef math::vec<kNormalVectorsLenght, VertexType> NormalVertex;
	typedef math::vec<kUVVectorsLenght, VertexType> UVVertex;
	typedef math::vec<kMaxBonesNumber, VertexType> BoneWeights;
	typedef math::vec<kMaxBonesNumber, IdType> BoneIds;
	
	struct VertexData
	{
		PositionVertex mPositionVertex;
		NormalVertex mNormalVertex;
		UVVertex mUVVertex;
		BoneWeights mBoneWeights;
		BoneIds mBoneIds;
	};
	
}
