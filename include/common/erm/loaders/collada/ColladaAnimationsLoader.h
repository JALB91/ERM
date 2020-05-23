#pragma once

#include "erm/loaders/collada/ColladaSkinData.h"

#include "erm/managers/ResourcesManager.h"

#include <map>
#include <mutex>
#include <string>

namespace tinyxml2 {
	class XMLDocument;
}

namespace erm {

	extern void ProcessAnimations(
		std::mutex& mutex,
		tinyxml2::XMLDocument& document,
		const std::map<std::string, ColladaSkinData>& skinsData,
		const char* path,
		Animations& animations);

}
