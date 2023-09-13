#pragma once

#include "erm/resources/loaders/collada/ColladaSkinData.h"

#include "erm/resources/ResourcesManager.h"

#include <map>
#include <mutex>
#include <string>
#include <string_view>

namespace tinyxml2 {
class XMLDocument;
}

namespace erm {

extern void ProcessScene(
	std::mutex& mutex,
	std::string_view path,
	tinyxml2::XMLDocument& document,
	std::vector<Skin>& skins,
	const std::map<std::string, ColladaSkinData>& skinsData);

}
