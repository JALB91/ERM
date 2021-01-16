#pragma once

#ifdef ERM_FBX_ENABLED

#	include <atomic>
#	include <mutex>

namespace erm {
	class Model;
	class ResourcesManager;
} // namespace erm

namespace erm {

	extern void ParseFBXModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		ResourcesManager& resourcesManager);

}

#endif