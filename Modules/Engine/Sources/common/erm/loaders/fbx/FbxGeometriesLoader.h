#pragma once

// clang-format off
#include "erm/loaders/fbx/FBXInclude.h"

#include "erm/rendering/data_structs/Bone.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>
// clang-format on

namespace erm {
class Model;
class ResourcesManager;
struct SkeletonAnimation;
} // namespace erm

namespace erm {

extern void ProcessGeometries(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxScene& scene,
	std::vector<std::unique_ptr<SkeletonAnimation>>& animations);

}
