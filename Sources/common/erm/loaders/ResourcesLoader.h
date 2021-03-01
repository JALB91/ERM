#pragma once

#include <atomic>
#include <deque>
#include <future>
#include <mutex>

namespace erm {
	class Device;
	class Model;
	class ResourcesManager;
} // namespace erm

namespace erm {

	class ResourcesLoader
	{
	public:
		ResourcesLoader(Device& device);
		~ResourcesLoader();

		void OnUpdate();
		void OnPreRender();
		void OnPostRender();

		bool IsStillLoading(const Model& model) const;

		bool ParseModel(
			const char* path,
			ResourcesManager& resourcesManager,
			bool async = true);

	private:
		Device& mDevice;
		std::mutex mMutex;
		std::atomic<bool> mStop;
		std::deque<erm::Model*> mLoadingModels;
		std::deque<std::future<void>> mFutures;
	};

} // namespace erm
