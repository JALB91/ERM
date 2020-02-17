#pragma once

#include "managers/ResourcesManager.h"

#include <atomic>
#include <deque>
#include <future>
#include <mutex>

namespace erm {
	
	class Model;
	
	class ModelUtils
	{
	public:
		~ModelUtils();
		
		void OnUpdate();
		void OnRender();
		void OnPostRender();

		bool ParseModel(
			const char* path,
			Models& models,
			Materials& materials
		);
		
	private:
		std::mutex mMutex;
		std::atomic<bool> mStop;
		std::deque<erm::Model*> mLoadingModels;
		std::deque<std::future<void>> mFutures;
		
	};
	
}
