#include "utils/ModelUtils.h"

#include "rendering/data_structs/Mesh.h"
#include "rendering/data_structs/Model.h"

#include "utils/DaeModelUtils.h"
#include "utils/ObjModelUtils.h"

#include <iostream>
#include <thread>
#include <algorithm>
#include <filesystem>

namespace erm {
	
	ModelUtils::~ModelUtils()
	{
		mStop = true;
		for (const std::future<void>& future : mFutures)
		{
			future.wait();
		}
		mFutures.clear();
		mLoadingModels.clear();
	}

	void ModelUtils::OnUpdate()
	{
		if (!mMutex.try_lock())
		{
			return;
		}

		for (erm::Model* model : mLoadingModels)
		{
			for (Mesh& mesh : model->GetMeshes())
			{
				if (!mesh.IsReady())
				{
					mesh.Setup();
				}
			}
		}

		mMutex.unlock();

		for (int i = 0; i < static_cast<int>(mFutures.size()); ++i)
		{
			std::future<void>& future = mFutures[i];

			if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				mFutures.erase(mFutures.begin() + i);
				mLoadingModels.erase(mLoadingModels.begin() + i);
				--i;
			}
		}
	}
	
	void ModelUtils::OnRender()
	{
		mMutex.lock();
	}
	
	void ModelUtils::OnPostRender()
	{
		mMutex.unlock();
	}
	
	bool ModelUtils::ParseModel(
		const char* path,
		Models& models,
		Materials& materials,
		Skins& skins,
		Animations& animations
	)
	{
		if (!std::filesystem::exists(path))
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}

		std::string pathStr = path;
		std::string name = pathStr.substr(pathStr.rfind("/") + 1, pathStr.rfind("."));
		std::string extension = pathStr.substr(pathStr.rfind(".") + 1);

		Model& model = *models.emplace_back(std::make_unique<Model>(path, name.c_str()));
		mLoadingModels.emplace_back(&model);
		
		if (std::strcmp(extension.c_str(), "obj") == 0)
		{
			mFutures.emplace_back(
				std::async(
					std::launch::async,
					&ParseObjModel,
					std::ref(mMutex),
					std::ref(mStop),
					path,
					std::ref(model),
					std::ref(materials)
				)
			);
		}
		else if (std::strcmp(extension.c_str(), "dae") == 0)
		{
			mFutures.emplace_back(
				std::async(
					std::launch::async,
					&ParseDaeModel,
					std::ref(mMutex),
					std::ref(mStop),
					path,
					std::ref(model),
					std::ref(materials),
					std::ref(skins),
					std::ref(animations)
				)
			);
		}

		return true;
	}

}
