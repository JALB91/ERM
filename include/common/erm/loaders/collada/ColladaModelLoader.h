#pragma once

#include "erm/managers/ResourcesManager.h"

#include <atomic>
#include <mutex>

namespace erm {

	extern void ParseColladaModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		Materials& materials,
		Skins& skins,
		Animations& animations);

}
