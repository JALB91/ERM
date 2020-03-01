#pragma once

#include "loaders/collada/ColladaSkinData.h"

#include "managers/ResourcesManager.h"

#include <map>
#include <mutex>
#include <string>

namespace tinyxml2 {
	class XMLDocument;
}

namespace erm {
	
	extern void ProcessScene(
		std::mutex& mutex,
		tinyxml2::XMLDocument& document,
		Skins& skins,
		const std::map<std::string, ColladaSkinData>& skinsData
	);
	
}
