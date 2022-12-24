#pragma once

#include <atomic>
#include <future>
#include <mutex>
#include <string_view>
#include <vector>

namespace erm {
class Model;
class ResourcesManager;
} // namespace erm

namespace erm {

class ResourcesLoader
{
public:
	ResourcesLoader();
	~ResourcesLoader();

	void OnUpdate();
	void OnPreRender();
	void OnPostRender();

	bool IsStillLoading(const Model& model) const;

	bool ParseModel(
		std::string_view path,
		ResourcesManager& resourcesManager,
		bool async = true);

private:
	std::mutex mMutex;
	std::atomic<bool> mStop;
	std::vector<erm::Model*> mLoadingModels;
	std::vector<std::future<void>> mFutures;
};

} // namespace erm
