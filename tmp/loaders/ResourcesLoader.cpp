#include "erm/resources/loaders/ResourcesLoader.h"

#include "erm/resources/ResourcesManager.h"

#include "erm/resources/models/Model.h"

#include "erm/resources/loaders/assimp/AssimpModelLoader.h"
#include "erm/resources/loaders/collada/ColladaModelLoader.h"
#include "erm/resources/loaders/fbx/FBXModelLoader.h"
#include "erm/resources/loaders/obj/ObjModelLoader.h"

#include <erm/utils/Utils.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <thread>

namespace erm {

ResourcesLoader::ResourcesLoader()
	: mStop(false)
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

	for (int i = 0; i < static_cast<int>(mFutures.size()); ++i)
	{
		std::future<void>& future = mFutures[i];

		if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
//			mLoadingModels[i]->UpdateBuffers();
			mFutures.erase(mFutures.begin() + i);
			mLoadingModels.erase(mLoadingModels.begin() + i);
			--i;
		}
	}

	mMutex.unlock();
}

void ResourcesLoader::OnPreRender()
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
	std::string_view path,
	ResourcesManager& resourcesManager,
	bool async /*= true*/)
{
	if (!std::filesystem::exists(path))
	{
		std::cout << "No such file: " << path << std::endl;
		return false;
	}

	std::string_view name = path.substr(path.rfind("/") + 1, path.rfind("."));
	std::string_view extension = path.substr(path.rfind("."));

	Model* model = resourcesManager.RegisterResource(Model(name));
	
	ERM_ASSERT(model != nullptr);

	if (async)
		mLoadingModels.emplace_back(model);

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
		AssimpParseModel(mMutex, mStop, path, *model, resourcesManager);
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
					std::ref(*model),
					std::ref(resourcesManager)));
		}
		else
		{
			ParseObjModel(mMutex, mStop, path, *model, resourcesManager);
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
					std::ref(*model),
					std::ref(resourcesManager)));
		}
		else
		{
			ParseColladaModel(mMutex, mStop, path, *model, resourcesManager);
		}

		return true;
	}
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
					std::ref(*model),
					std::ref(resourcesManager)));
		}
		else
		{
			ParseFBXModel(mMutex, mStop, path, *model, resourcesManager);
		}
		return true;
	}
	return false;
#endif
}

} // namespace erm
