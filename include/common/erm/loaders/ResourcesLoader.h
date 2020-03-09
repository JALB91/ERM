#pragma once

#include "erm/managers/ResourcesManager.h"

#include <atomic>
#include <deque>
#include <future>
#include <mutex>

namespace erm {
	
	class Model;
	
	class ResourcesLoader
	{
	public:
		~ResourcesLoader();
		
		void OnUpdate();
		void OnRender();
		void OnPostRender();

		bool ParseModel(
			const char* path,
			Models& models,
			Materials& materials,
			Skins& skins,
			Animations& animations
		);
		
	private:
		std::mutex mMutex;
		std::atomic<bool> mStop;
		std::deque<erm::Model*> mLoadingModels;
		std::deque<std::future<void>> mFutures;
		
	};
	
}
