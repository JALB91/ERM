#pragma once

#ifdef ERM_FBX_ENABLED

#	include "erm/loaders/fbx/FBXInclude.h"

#	include <atomic>
#	include <mutex>

namespace erm {
	class ResourcesManager;
	struct Material;
} // namespace erm

namespace erm {

	Material* ParseFBXMaterial(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		FbxMesh* pMesh,
		ResourcesManager& resourcesManager);

}

#endif