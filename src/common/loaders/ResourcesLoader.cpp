#include "erm/loaders/ResourcesLoader.h"

#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/Model.h"

#include "erm/loaders/collada/ColladaModelLoader.h"
#include "erm/loaders/obj/ObjModelLoader.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <thread>

namespace erm {

	ResourcesLoader::ResourcesLoader(Device& device)
		: mDevice(device)
		, mStop(false)
	{}

	ResourcesLoader::~ResourcesLoader()
	{
		mStop = true;
		for (const std::future<void>& future : mFutures)
		{
			future.wait();
		}
		mFutures.clear();
		mLoadingModels.clear();
	}

	void ResourcesLoader::OnUpdate()
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

	void ResourcesLoader::OnRender()
	{
		mMutex.lock();
	}

	void ResourcesLoader::OnPostRender()
	{
		mMutex.unlock();
	}

	bool ResourcesLoader::ParseModel(
		const char* path,
		Models& models,
		Materials& materials,
		Skins& skins,
		Animations& animations)
	{
		if (!std::filesystem::exists(path))
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}

		std::string pathStr = path;
		std::string name = pathStr.substr(pathStr.rfind("/") + 1, pathStr.rfind("."));
		std::string extension = pathStr.substr(pathStr.rfind(".") + 1);

		Model& model = *models.emplace_back(std::make_unique<Model>(mDevice, path, name.c_str()));
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
					std::ref(materials)));
		}
		else if (std::strcmp(extension.c_str(), "dae") == 0)
		{
			mFutures.emplace_back(
				std::async(
					std::launch::async,
					&ParseColladaModel,
					std::ref(mMutex),
					std::ref(mStop),
					path,
					std::ref(model),
					std::ref(materials),
					std::ref(skins),
					std::ref(animations)));
		}

		return true;
	}

} // namespace erm
