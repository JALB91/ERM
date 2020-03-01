#pragma once

#include "managers/ResourcesManager.h"

#include <mutex>
#include <atomic>

namespace erm {
	
	extern void ParseColladaModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		Materials& materials,
		Skins& skins,
		Animations& animations
	);
	
}
