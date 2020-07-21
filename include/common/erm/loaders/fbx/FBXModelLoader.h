#pragma once

#include "erm/managers/ResourcesManager.h"

#include <atomic>
#include <mutex>

namespace erm {

	extern void ParseFBXModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		Materials& materials);

}
