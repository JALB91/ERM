#pragma once

#include <atomic>
#include <mutex>
#include <string_view>

namespace erm {
class Model;
class ResourcesManager;
} // namespace erm

namespace erm {

extern void ParseObjModel(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	std::string_view path,
	Model& model,
	ResourcesManager& resourcesManager);

}
