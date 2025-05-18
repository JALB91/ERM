#include "erm/assets/AssetsManager.h"

#include "erm/assets/AssetsRepo.h"
#include "erm/assets/animations/SkeletonAnimation.h"
#include "erm/assets/data_structs/Skeleton.h"
#include "erm/assets/materials/Material.h"
#include "erm/assets/materials/PBMaterial.h"
#include "erm/assets/models/Mesh.h"
#include "erm/assets/models/MeshUtils.h"
#include "erm/assets/models/Model.h"
#include "erm/assets/shaders/ShaderProgram.h"
#include "erm/assets/textures/CubeMap.h"
#include "erm/assets/textures/Texture.h"

#include <erm/modules_lib/ObjectRegistry.h>

#include <algorithm>

namespace erm {

AssetsManager::AssetsManager()
: mAssetsRepo(ObjectRegistry::require<AssetsRepo>())
{}

void AssetsManager::loadDefaultAssets()
{
	mAssetsRepo.registerAsset(StringID("Defaults/Triangle"), std::make_unique<Mesh>(MeshUtils::createTriangle()));
	mAssetsRepo.registerAsset(StringID("Defaults/Square"), std::make_unique<Mesh>(MeshUtils::createSquare()));
	mAssetsRepo.registerAsset(StringID("Defaults/Cube"), std::make_unique<Mesh>(MeshUtils::createCube()));
	mAssetsRepo.registerAsset(StringID("Defaults/Sphere"), std::make_unique<Mesh>(MeshUtils::createSphere()));
	mAssetsRepo.registerAsset(StringID("Defaults/Spike"), std::make_unique<Mesh>(MeshUtils::createSpike()));
	mAssetsRepo.registerAsset(StringID("Defaults/Grid"), std::make_unique<Mesh>(MeshUtils::createGrid()));

	for (const auto& [id, _] : mAssetsRepo.getAllAssetsOfType<Mesh>())
	{
		Model model;
		model.mMeshes.emplace_back(id);

		mAssetsRepo.registerAsset(id, std::make_unique<Model>(std::move(model)));
	}
}

}