#pragma once

#include "rendering/VertexData.h"
#include "rendering/IndexData.h"

#include <string>
#include <vector>

namespace erm {
	
	class Model;
	
	class ModelUtils
	{
	public:
		static Model ParseModel(const char* path);
		static void ParseFace(
			std::vector<VertexData>& oVertices,
			const std::vector<Vertex>& positions,
			const std::vector<UVVertex>& tPositions,
			const std::vector<NormalVertex>& nPositions,
			const std::vector<std::string>& splitted
		);
		static void Triangulate(
			std::vector<IndexData>& oIndices,
			const std::vector<VertexData>& vertices
		);
	};
	
}
