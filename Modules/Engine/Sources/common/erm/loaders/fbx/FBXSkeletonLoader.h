#pragma once

#include "erm/loaders/fbx/FBXInclude.h"

#include "erm/rendering/data_structs/Bone.h"

#include <atomic>
#include <memory>
#include <mutex>

namespace erm {
class ResourcesManager;
}

namespace erm {

void ProcessSkeleton(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxScene& scene);

}
