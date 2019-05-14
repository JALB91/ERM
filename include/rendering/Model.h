#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace erm {
	
	class Mesh;
	
	class Model
	{
	public:
		Model(std::vector<Mesh> meshes);
		~Model();
		
		Model(const Model& other) = delete;
		Model(Model&& other);
		
		Model& operator=(const Model& other) = delete;
		Model& operator=(Model&& other);
		
		inline const std::vector<Mesh>& GetMeshes() const { return mMeshes; }
		
	private:
		std::vector<Mesh> mMeshes;
		
	};
	
}
