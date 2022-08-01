#pragma once

#include "erm/loaders/fbx/FBXInclude.h"

#include <mutex>

namespace erm {
class ResourcesManager;
struct Material;
} // namespace erm

namespace erm {

Material* ParseFBXMaterial(
	std::mutex& mutex,
	const char* path,
	FbxMesh* pMesh,
	ResourcesManager& resourcesManager);

}
