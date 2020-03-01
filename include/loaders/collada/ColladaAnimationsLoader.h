#pragma once

#include "managers/ResourcesManager.h"

#include <mutex>
#include <string>
#include <vector>

namespace tinyxml2 {
	class XMLDocument;
}

namespace erm {
	
	extern void ProcessAnimations(
		std::mutex& mutex,
		tinyxml2::XMLDocument& document,
		const std::vector<std::string>& boneNames,
		const char* path,
		Animations& animations
	);
	
}
