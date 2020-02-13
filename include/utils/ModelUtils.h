#pragma once

#include "managers/ResourcesManager.h"

#include "rendering/VertexData.h"
#include "rendering/IndexData.h"

#include <memory>
#include <string>
#include <vector>

namespace erm {
	
	class Mesh;
	class Model;
	struct Material;
	
	class ModelUtils
	{
	public:
		static void OnUpdate();
		static void OnRender();
		static void OnPostRender();
		static void OnDestroy();

		static bool ParseModel(
			const char* path,
			Models& models,
			Materials& materials
		);
		
	private:
		static bool ParseMaterialsLib(
			const char* path,
			Materials& materials
		);
		
		static void ParseModelInt(
			const char* path,
			Model& model,
			Materials& materials
		);

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
		
		static void AddMesh(
			Model& model,
			std::vector<VertexData>& vertices,
			std::vector<IndexData>& indices,
			Material* material,
			std::string& meshName
		);
		
		static Mesh CreateMesh(
			const std::vector<VertexData>& vertices,
			const std::vector<IndexData>& indices,
			Material* material,
			const std::string& name
		);
		
	};
	
}
