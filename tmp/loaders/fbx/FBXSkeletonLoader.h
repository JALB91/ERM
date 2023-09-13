#pragma once

#include "erm/resources/loaders/fbx/FBXInclude.h"
#include "erm/resources/data_structs/Bone.h"

#include <atomic>
#include <memory>
#include <mutex>

namespace erm {

void ProcessSkeleton(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	std::unique_ptr<BonesTree>& bonesTree,
	FbxScene& scene);

}
