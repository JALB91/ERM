#pragma once

#include <glm/glm.hpp>

namespace erm {
	
	static constexpr unsigned int kVectorsLenght = 3;
	static constexpr unsigned int kNormalVectorsLenght = 3;
	static constexpr unsigned int kTextureVectorsLenght = 2;
	
	typedef float VertexType;
	
	typedef glm::vec<kVectorsLenght, VertexType> Vertex;
	typedef glm::vec<kNormalVectorsLenght, VertexType> NormalVertex;
	typedef glm::vec<kTextureVectorsLenght, VertexType> TextureVertex;
	
	struct VertexData
	{
		Vertex mVertex;
		NormalVertex mNormalVertex;
		TextureVertex mTextureVertex;
	};
	
}
