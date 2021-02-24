#include "erm/loaders/ResourcesLoader.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/Model.h"

#include "erm/loaders/assimp/AssimpModelLoader.h"
#include "erm/loaders/collada/ColladaModelLoader.h"
#ifdef ERM_FBX_ENABLED
#	include "erm/loaders/fbx/FBXModelLoader.h"
#endif
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

	bool ResourcesLoader::IsStillLoading(const Model& model) const
	{
		return std::find(mLoadingModels.begin(), mLoadingModels.end(), &model) != mLoadingModels.end();
	}

	bool ResourcesLoader::ParseModel(
		const char* path,
		ResourcesManager& resourcesManager,
		bool async /*= true*/)
	{
		if (!std::filesystem::exists(path))
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}

		std::string pathStr = path;
		std::string name = pathStr.substr(pathStr.rfind("/") + 1, pathStr.rfind("."));
		std::string extension = pathStr.substr(pathStr.rfind("."));

		Model& model = *resourcesManager.GetModels().emplace_back(std::make_unique<Model>(mDevice, path, name.c_str()));
		if (async)
			mLoadingModels.emplace_back(&model);

#ifdef ERM_ASSIMP_ENABLED
		if (async)
		{
			mFutures.emplace_back(
				std::async(
					std::launch::async,
					&AssimpParseModel,
					std::ref(mMutex),
					std::ref(mStop),
					path,
					std::ref(model),
					std::ref(resourcesManager)));
		}
		else
		{
			AssimpParseModel(mMutex, mStop, path, model, resourcesManager);
			for (Mesh& mesh : model.GetMeshes())
				mesh.Setup();
		}

		return true;
#else
		if (Utils::CompareNoCaseSensitive(extension, ".obj"))
		{
			if (async)
			{
				mFutures.emplace_back(
					std::async(
						std::launch::async,
						&ParseObjModel,
						std::ref(mMutex),
						std::ref(mStop),
						path,
						std::ref(model),
						std::ref(resourcesManager)));
			}
			else
			{
				ParseObjModel(mMutex, mStop, path, model, resourcesManager);
				for (Mesh& mesh : model.GetMeshes())
					mesh.Setup();
			}

			return true;
		}
		else if (Utils::CompareNoCaseSensitive(extension, ".dae"))
		{
			if (async)
			{
				mFutures.emplace_back(
					std::async(
						std::launch::async,
						&ParseColladaModel,
						std::ref(mMutex),
						std::ref(mStop),
						path,
						std::ref(model),
						std::ref(resourcesManager)));
			}
			else
			{
				ParseColladaModel(mMutex, mStop, path, model, resourcesManager);
				for (Mesh& mesh : model.GetMeshes())
					mesh.Setup();
			}

			return true;
		}
#	ifdef ERM_FBX_ENABLED
		else if (Utils::CompareNoCaseSensitive(extension, ".fbx"))
		{
			if (async)
			{
				mFutures.emplace_back(
					std::async(
						std::launch::async,
						&ParseFBXModel,
						std::ref(mMutex),
						std::ref(mStop),
						path,
						std::ref(model),
						std::ref(resourcesManager)));
			}
			else
			{
				ParseFBXModel(mMutex, mStop, path, model, resourcesManager);
				for (Mesh& mesh : model.GetMeshes())
					mesh.Setup();
			}

			return true;
		}
#	endif

		return false;
#endif
	}

} // namespace erm
