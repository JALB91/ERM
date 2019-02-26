#include "Model.h"
#include "Renderer.h"
#include "VertexData.h"
#include "Utils.h"
#include "Mesh.h"

#include <iostream>

namespace erm {
	
	Model::Model(std::vector<Mesh> meshes)
		: mTranslation(0.0f)
		, mRotation(0.0f)
		, mMeshes(std::forward<std::vector<Mesh>>(meshes))
	{}
	
	Model::~Model()
	{}
	
	Model::Model(Model&& other)
	: mTranslation(other.mTranslation)
	, mRotation(other.mRotation)
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
		mMeshes = other.mMeshes;
		
		other.mMeshes.clear();
		
		return *this;
	}
	
}
