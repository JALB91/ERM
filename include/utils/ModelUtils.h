#pragma once

#include "rendering/VertexData.h"
#include "rendering/IndexData.h"

#include <string>
#include <vector>
#include <deque>

namespace erm {
	
	class Mesh;
	class Model;
	struct Material;
	
	class ModelUtils
	{
	public:
		static bool ParseModel(
			const char* path,
			std::deque<Model>& modelsContainer,
			std::deque<Material>& materialsContainer
		);
		
		static bool ParseMaterialsLib(
			const char* path,
			std::deque<Material>& materialsContainer
		);
		
	private:
		static void ParseFace(
			std::deque<VertexData>& oVertices,
			const std::deque<Vertex>& positions,
			const std::deque<UVVertex>& tPositions,
			const std::deque<NormalVertex>& nPositions,
			const std::vector<std::string>& splitted
		);
		static void Triangulate(
			std::deque<IndexData>& oIndices,
			const std::deque<VertexData>& vertices
		);
		static Mesh CreateMesh(
			const std::deque<VertexData>& vertices,
			const std::deque<IndexData>& indices,
			Material* material,
			const std::string& name
		);
		
	};
	
}
