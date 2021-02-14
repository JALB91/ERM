#pragma once

#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/loaders/fbx/FBXInclude.h"

#include "erm/rendering/data_structs/Bone.h"

#include <atomic>
#include <memory>
#include <mutex>
// clang-format on

namespace erm {
	class Model;
	class ResourcesManager;
} // namespace erm

namespace erm {

	extern void ProcessGeometries(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		ResourcesManager& resourcesManager,
		std::unique_ptr<BonesTree>& bonesTree,
		FbxScene& scene);

}

#endif