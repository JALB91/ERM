#pragma once

#ifdef ERM_ASSIMP_ENABLED

#	include <atomic>
#	include <mutex>

namespace erm {
class Model;
class ResourcesManager;
} // namespace erm

namespace erm {

void AssimpParseModel(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	ResourcesManager& resourcesManager);

}

#endif