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
#include "erm/rendering/buffers/IndexData.h"
#include "erm/rendering/buffers/VertexData.h"
#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <map>
#include <vector>
// clang-format on

namespace erm {

	void ParseFBXModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		ResourcesManager& resourcesManager)
	{
		FbxManager* lSdkManager = FbxManager::Create();

		FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
		auto& t = static_cast<FbxIOBase*>(lImporter)->GetStatus();

		if (!lImporter->Initialize(path, -1, lSdkManager->GetIOSettings()))
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		}

		FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
		lImporter->Import(lScene);
		lImporter->Destroy();

		std::unique_ptr<BonesTree> bonesTree;

		ProcessSkeleton(mutex, stop, bonesTree, *lScene);
		ProcessGeometries(mutex, stop, path, model, resourcesManager, bonesTree, *lScene);

		if (bonesTree)
		{
			mutex.lock();
			resourcesManager.GetSkins().emplace_back(std::make_unique<Skin>(path, path, std::move(bonesTree)));
			mutex.unlock();
		}

		lSdkManager->Destroy();
	}

} // namespace erm

#endif