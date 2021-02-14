#pragma once

#include "erm/loaders/collada/ColladaSkinData.h"

#include <atomic>
#include <map>
#include <mutex>
#include <string>

namespace tinyxml2 {
	class XMLDocument;
}
namespace erm {
	class Model;
} // namespace erm

namespace erm {

	extern void ProcessGeometries(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		tinyxml2::XMLDocument& document,
		Model& model,
		const std::map<std::string, ColladaSkinData>& skinsData);

}
