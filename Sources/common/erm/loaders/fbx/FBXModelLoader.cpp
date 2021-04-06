#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/loaders/fbx/FBXModelLoader.h"

#include "erm/loaders/fbx/FBXInclude.h"
#include "erm/loaders/fbx/FBXMaterialLoader.h"
#include "erm/loaders/fbx/FBXSkeletonData.h"
#include "erm/loaders/fbx/FBXSkeletonLoader.h"
#include "erm/loaders/fbx/FbxUtils.h"
#include "erm/loaders/fbx/FbxGeometriesLoader.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/math/vec.h"

#include "erm/rendering/data_structs/Skin.h"
#include "erm/rendering/data_structs/IndexData.h"
#include "erm/rendering/data_structs/VertexData.h"
#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/Model.h"

#include <map>
#include <vector>
// clang-format on

namespace {

static std::mutex sFbxMutex;
static FbxManager* sManager = nullptr;
static FbxIOSettings* sIOSettings = nullptr;
static FbxImporter* sImporter = nullptr;

void InitFBXManager()
{
	ASSERT(!sManager && !sIOSettings && !sImporter);

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
	sFbxMutex.unlock();

	std::unique_ptr<BonesTree> bonesTree;

	ProcessSkeleton(mutex, stop, bonesTree, *lScene);
	ProcessGeometries(mutex, stop, path, model, resourcesManager, bonesTree, *lScene);

	if (bonesTree)
	{
		mutex.lock();
		resourcesManager.GetSkins().emplace_back(std::make_unique<Skin>(path, path, std::move(bonesTree)));
		mutex.unlock();
	}
}

} // namespace erm

#endif
