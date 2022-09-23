#include "erm/loaders/fbx/FBXModelLoader.h"

#include "erm/loaders/fbx/FBXInclude.h"
#include "erm/loaders/fbx/FBXMaterialLoader.h"
#include "erm/loaders/fbx/FBXSkeletonData.h"
#include "erm/loaders/fbx/FBXSkeletonLoader.h"
#include "erm/loaders/fbx/FbxUtils.h"
#include "erm/loaders/fbx/FbxGeometriesLoader.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/math/vec.h"

#include "erm/rendering/animations/SkeletonAnimation.h"
#include "erm/rendering/data_structs/Skin.h"
#include "erm/rendering/data_structs/IndexData.h"
#include "erm/rendering/data_structs/VertexData.h"
#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/Model.h"

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
	const char* path,
	Model& model,
	ResourcesManager& resourcesManager)
{
	sFbxMutex.lock();
	if (!sManager)
		InitFBXManager();

	if (!sImporter->Initialize(path, -1, sManager->GetIOSettings()))
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
