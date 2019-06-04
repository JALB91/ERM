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
		static const Model& ParseModel(const char* path);
		
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
			const Material& material
		);
		
		static void ParseMaterialsLib(const char* path);
		
		static std::deque<Model> mLoadedModels;
		static std::deque<Material> mLoadedMaterials;
		
	};
	
}
