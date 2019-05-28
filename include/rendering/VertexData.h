#pragma once

#include <glm/glm.hpp>

namespace erm {
	
	static constexpr unsigned int kVectorsLenght = 3;
	static constexpr unsigned int kNormalVectorsLenght = 3;
	static constexpr unsigned int kTextureVectorsLenght = 2;
	
	typedef float VertexType;
	typedef glm::vec<kVectorsLenght, VertexType> Vertex;
	
	typedef float NormalVertexType;
	typedef glm::vec<kNormalVectorsLenght, NormalVertexType> NormalVertex;
	
	typedef float TextureVertexType;
	typedef glm::vec<kTextureVectorsLenght, TextureVertexType> TextureVertex;
	
	struct VertexData
	{
		Vertex mVertex;
		NormalVertex mNormalVertex;
		TextureVertex mTextureVertex;
	};
	
}
