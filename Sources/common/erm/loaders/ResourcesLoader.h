#pragma once

#include "erm/managers/ResourcesManager.h"

#include <atomic>
#include <deque>
#include <future>
#include <mutex>

namespace erm {
	class Device;
	class Model;
} // namespace erm

namespace erm {

	class ResourcesLoader
	{
	public:
		ResourcesLoader(Device& device);
		~ResourcesLoader();

		void OnUpdate();
		void OnRender();
		void OnPostRender();

		bool ParseModel(
			const char* path,
			ResourcesManager& resourcesManager);

	private:
		Device& mDevice;
		std::mutex mMutex;
		std::atomic<bool> mStop;
		std::deque<erm::Model*> mLoadingModels;
		std::deque<std::future<void>> mFutures;
	};

} // namespace erm
