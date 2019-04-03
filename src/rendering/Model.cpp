#include "rendering/Model.h"
#include "rendering/Renderer.h"
#include "rendering/VertexData.h"
#include "utils/Utils.h"
#include "rendering/Mesh.h"

#include <iostream>

namespace erm {
	
	Model::Model(std::vector<Mesh> meshes)
		: ITransformable()
		, mMeshes(std::forward<std::vector<Mesh>>(meshes))
	{}
	
	Model::~Model()
	{}
	
	Model::Model(Model&& other)
		: ITransformable()
		, mMeshes(std::move(other.mMeshes))
	{}
	
	Model& Model::operator=(Model&& other)
	{
		if (this == &other)
		{
			return *this;
		}
		
		mTranslation = other.mTranslation;
		mRotation = other.mRotation;
		mScale = other.mScale;
		mMeshes = other.mMeshes;
		
		other.mMeshes.clear();
		
		return *this;
	}
	
}
