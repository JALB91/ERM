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

extern void ProcessAnimations(
	std::mutex& mutex,
	tinyxml2::XMLDocument& document,
	const std::map<std::string, ColladaSkinData>& skinsData,
	std::string_view path,
	std::vector<SkeletonAnimation>& animations);

}
