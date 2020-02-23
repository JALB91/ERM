#pragma once

#include "math/vec.h"

#include <glm/glm.hpp>

namespace erm {
	
	static constexpr unsigned int kPositionVectorsLenght = 3;
	static constexpr unsigned int kNormalVectorsLenght = 3;
	static constexpr unsigned int kUVVectorsLenght = 2;
	
	typedef float VertexType;
	
	typedef math::vec<kPositionVectorsLenght, VertexType> PositionVertex;
	typedef math::vec<kNormalVectorsLenght, VertexType> NormalVertex;
	typedef math::vec<kUVVectorsLenght, VertexType> UVVertex;
	
	struct VertexData
	{
		PositionVertex mPositionVertex;
		NormalVertex mNormalVertex;
		UVVertex mUVVertex;
	};
	
}
