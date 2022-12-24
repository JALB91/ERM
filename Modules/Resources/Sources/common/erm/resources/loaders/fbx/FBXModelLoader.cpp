#include "erm/resources/loaders/fbx/FBXModelLoader.h"

#include "erm/resources/loaders/fbx/FBXInclude.h"
#include "erm/resources/loaders/fbx/FBXMaterialLoader.h"
#include "erm/resources/loaders/fbx/FBXSkeletonData.h"
#include "erm/resources/loaders/fbx/FBXSkeletonLoader.h"
#include "erm/resources/loaders/fbx/FbxUtils.h"
#include "erm/resources/loaders/fbx/FbxGeometriesLoader.h"

#include "erm/resources/ResourcesManager.h"

#include "erm/math/vec.h"

#include "erm/resources/animations/SkeletonAnimation.h"
#include "erm/resources/data_structs/Skin.h"
#include "erm/resources/data_structs/Bone.h"
#include "erm/resources/models/IndexTypes.h"
#include "erm/resources/models/VertexData.h"
#include "erm/resources/models/Model.h"

#include <map>
#include <vector>

namespace {

static std::mutex sFbxMutex;
static FbxManager* sManager = nullptr;
static FbxIOSettings* sIOSettings = nullptr;
static FbxImporter* sImporter = nullptr;

void InitFBXManager()
{
	ERM_ASSERT(!sManager && !sIOSettings && !sImporter);

	sManager = FbxManager::Create();

	sIOSettings = FbxIOSettings::Create(sManager, IOSROOT);
	sManager->SetIOSettings(sIOSettings);

	sImporter = FbxImporter::Create(sManager, "");
}

} // namespace

namespace erm {

void ParseFBXModel(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	std::string_view path,
	Model& model,
	ResourcesManager& resourcesManager)
{
	sFbxMutex.lock();
	if (!sManager)
		InitFBXManager();

	std::string sPath (path.data(), path.size());
	if (!sImporter->Initialize(sPath.c_str(), -1, sManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", sImporter->GetStatus().GetErrorString());
	}

	FbxScene* lScene = FbxScene::Create(sManager, "myScene");
	sImporter->Import(lScene);

	std::unique_ptr<BonesTree> bonesTree;
	std::vector<std::unique_ptr<SkeletonAnimation>> animations;

	ProcessSkeleton(mutex, stop, bonesTree, *lScene);
	ProcessGeometries(mutex, stop, path, model, resourcesManager, bonesTree, *lScene, animations);

	if (bonesTree)
	{
		mutex.lock();
		resourcesManager.GetSkins().emplace_back(std::make_unique<Skin>(path, path, std::move(bonesTree)));
		mutex.unlock();
	}

	if (!animations.empty())
	{
		mutex.lock();
		for (std::unique_ptr<SkeletonAnimation>& animation : animations)
			resourcesManager.GetAnimations().emplace_back(std::move(animation));
		mutex.unlock();
	}
	
	sFbxMutex.unlock();
}

} // namespace erm
