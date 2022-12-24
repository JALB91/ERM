#pragma once

#include "erm/resources/loaders/fbx/FBXInclude.h"
#include "erm/resources/data_structs/Bone.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string_view>
#include <vector>

namespace erm {
class Model;
class ResourcesManager;
struct SkeletonAnimation;
} // namespace erm

namespace erm {

extern void ProcessGeometries(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	std::string_view path,
	Model& model,
	ResourcesManager& resourcesManager,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxScene& scene,
	std::vector<std::unique_ptr<SkeletonAnimation>>& animations);

}
