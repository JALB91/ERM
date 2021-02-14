#pragma once

#include <atomic>
#include <mutex>

namespace erm {
	class ResourcesManager;
}

namespace erm {

	extern bool ParseMaterialsLib(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		ResourcesManager& resourcesManager);

}