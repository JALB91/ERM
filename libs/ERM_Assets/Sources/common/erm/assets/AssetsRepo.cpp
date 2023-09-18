#include "erm/assets/AssetsRepo.h"

#include <algorithm>

namespace erm {

void AssetsRepo::LoadDefaultResources()
{
	mMeshes.reserve(10);
	
	mMeshes.insert_or_assign(StringID("Defaults/Triangle"), MeshUtils::CreateTriangle());
	mMeshes.insert_or_assign(StringID("Defaults/Square"), MeshUtils::CreateSquare());
	mMeshes.insert_or_assign(StringID("Defaults/Cube"), MeshUtils::CreateCube());
	mMeshes.insert_or_assign(StringID("Defaults/Sphere"), MeshUtils::CreateSphere());
	mMeshes.insert_or_assign(StringID("Defaults/Spike"), MeshUtils::CreateSpike());
	mMeshes.insert_or_assign(StringID("Defaults/Grid"), MeshUtils::CreateGrid());
	
	mModels.reserve(mMeshes.size());
	
	for (const auto& [id, mesh] : mMeshes) {
		Model model;
		model.mMeshes.emplace_back(id);
		
		mModels.insert_or_assign(id, std::move(model));
	}
}

} // namespace erm
