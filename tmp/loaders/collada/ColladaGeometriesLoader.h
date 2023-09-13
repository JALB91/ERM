#pragma once

#include "erm/resources/loaders/collada/ColladaSkinData.h"

#include <atomic>
#include <map>
#include <mutex>
#include <string>

namespace tinyxml2 {
class XMLDocument;
}
namespace erm {
class Model;
class ResourcesManager;
} // namespace erm

namespace erm {

extern void ProcessGeometries(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	tinyxml2::XMLDocument& document,
	Model& model,
	ResourcesManager& resourcesManager,
	const std::map<std::string, ColladaSkinData>& skinsData);

}
