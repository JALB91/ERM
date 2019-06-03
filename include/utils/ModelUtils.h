#pragma once

#include "rendering/VertexData.h"
#include "rendering/IndexData.h"

#include <string>
#include <vector>
#include <deque>

namespace erm {
	
	class Model;
	struct Material;
	
	class ModelUtils
	{
	public:
		static const Model& ParseModel(const char* path);
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
		
		static void ParseMaterialsLib(const char* path);
		
		static inline const std::deque<Model>& GetModels()
		{
			return mLoadedModels;
		}
		
		static inline const std::deque<Material>& GetMaterials()
		{
			return mLoadedMaterials;
		}
		
	private:
		static std::deque<Model> mLoadedModels;
		static std::deque<Material> mLoadedMaterials;
		
	};
	
}
