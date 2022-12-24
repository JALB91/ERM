#pragma once

#include "erm/resources/loaders/fbx/FBXInclude.h"

#include <mutex>
#include <string_view>

namespace erm {
class ResourcesManager;
struct Material;
} // namespace erm

namespace erm {

Material* ParseFBXMaterial(
	std::mutex& mutex,
	std::string_view path,
	FbxMesh* pMesh,
	ResourcesManager& resourcesManager);

}
