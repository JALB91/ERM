#pragma once

#include "math/vec.h"

#include <glm/glm.hpp>

namespace erm {
	
	static constexpr unsigned int kVectorsLenght = 3;
	static constexpr unsigned int kUVVectorsLenght = 2;
	static constexpr unsigned int kNormalVectorsLenght = 3;
	
	typedef float VertexType;
	
	typedef math::vec<kVectorsLenght, VertexType> Vertex;
	typedef math::vec<kUVVectorsLenght, VertexType> UVVertex;
	typedef math::vec<kNormalVectorsLenght, VertexType> NormalVertex;
	
	struct VertexData
	{
		Vertex mVertex;
		UVVertex mUVVertex;
		NormalVertex mNormalVertex;
	};
	
}
