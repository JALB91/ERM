#pragma once

#include "rendering/VertexData.h"
#include "rendering/IndexData.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace erm {
	
	class Model;
	
	class ModelUtils
	{
	public:
		static Model ParseModel(const char* path);
		
	private:
		static void ParseFace(
			std::vector<VertexData>& oVertices,
			const std::vector<Vertex>& positions,
			const std::vector<NormalVertex>& nPositions,
			const std::vector<TextureVertex>& tPositions,
			const std::vector<std::string>& splitted
		);
		static void Triangulate(
			std::vector<IndexData>& oIndices,
			const std::vector<VertexData>& vertices
		);
		
	};
	
}
