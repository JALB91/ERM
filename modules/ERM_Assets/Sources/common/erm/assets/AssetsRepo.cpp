#include "erm/assets/AssetsRepo.h"

#include <algorithm>

namespace erm {

void AssetsRepo::loadDefaultResources()
{
	mMeshes.reserve(10);
	
	mMeshes.insert_or_assign(StringID("Defaults/Triangle"), MeshUtils::createTriangle());
	mMeshes.insert_or_assign(StringID("Defaults/Square"), MeshUtils::createSquare());
	mMeshes.insert_or_assign(StringID("Defaults/Cube"), MeshUtils::createCube());
	mMeshes.insert_or_assign(StringID("Defaults/Sphere"), MeshUtils::createSphere());
	mMeshes.insert_or_assign(StringID("Defaults/Spike"), MeshUtils::createSpike());
	mMeshes.insert_or_assign(StringID("Defaults/Grid"), MeshUtils::createGrid());
	
	mModels.reserve(mMeshes.size());
	
	for (const auto& [id, mesh] : mMeshes) {
		Model model;
		model.mMeshes.emplace_back(id);
		
		mModels.insert_or_assign(id, std::move(model));
	}
}

} // namespace erm
