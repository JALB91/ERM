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

AssetsManager::AssetsManager() noexcept
: mAssetsRepo(ObjectRegistry::require<AssetsRepo>())
{}

}
