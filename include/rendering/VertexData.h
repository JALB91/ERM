#pragma once

#include <glm/glm.hpp>

namespace erm {
	
	static constexpr unsigned int kVectorsLenght = 3;
	static constexpr unsigned int kUVVectorsLenght = 2;
	static constexpr unsigned int kNormalVectorsLenght = 3;
	
	typedef float VertexType;
	
	typedef glm::vec<kVectorsLenght, VertexType> Vertex;
	typedef glm::vec<kUVVectorsLenght, VertexType> UVVertex;
	typedef glm::vec<kNormalVectorsLenght, VertexType> NormalVertex;
	
	struct VertexData
	{
		Vertex mVertex;
		UVVertex mUVVertex;
		NormalVertex mNormalVertex;
	};
	
}
