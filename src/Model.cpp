#include "Model.h"
#include "Renderer.h"
#include "VertexData.h"
#include "Utils.h"

namespace erm {
	
	Model::Model(std::vector<Mesh> meshes)
		: mTranslation(0.0f)
		, mRotation(0.0f)
		, mMeshes(meshes)
	{}
	
	Model::~Model()
	{}
	
}
