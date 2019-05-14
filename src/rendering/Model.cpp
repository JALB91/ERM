#include "rendering/Model.h"
#include "rendering/Renderer.h"
#include "rendering/VertexData.h"
#include "rendering/Mesh.h"

#include "utils/Utils.h"

#include <iostream>

namespace erm {
	
	Model::Model(std::vector<Mesh> meshes)
		: mMeshes(std::forward<std::vector<Mesh>>(meshes))
	{}
	
	Model::~Model()
	{}
	
	Model::Model(Model&& other)
		: mMeshes(std::move(other.mMeshes))
	{}
	
	Model& Model::operator=(Model&& other)
	{
		if (this == &other)
		{
			return *this;
		}
		
		mMeshes = other.mMeshes;
		
		other.mMeshes.clear();
		
		return *this;
	}
	
}
