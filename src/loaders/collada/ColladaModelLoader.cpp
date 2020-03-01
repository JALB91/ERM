#include "loaders/collada/ColladaModelLoader.h"

#include "loaders/collada/ColladaLoaderUtils.h"
#include "loaders/collada/ColladaSceneLoader.h"
#include "loaders/collada/ColladaSkeletonLoader.h"
#include "loaders/collada/ColladaGeometriesLoader.h"
#include "loaders/collada/ColladaAnimationsLoader.h"

#include <tinyxml2.h>

#include <iostream>

using namespace tinyxml2;

namespace erm {
	
	void ParseColladaModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		Materials& /*materials*/,
		Skins& skins,
		Animations& animations
	)
	{
		XMLDocument document;
		XMLError error = document.LoadFile(path);
		
		if (error != XMLError::XML_SUCCESS)
		{
			std::cout << "Error %d while reading dae file %s" << error << path << std::endl;
			return;
		}
		
		std::vector<BoneIds> boneIds;
		std::vector<BoneWeights> boneWeights;
		std::vector<std::string> boneNames;
		
		ProcessSkeleton(document, boneIds, boneWeights, boneNames);
		ProcessGeometries(mutex, stop, document, boneIds, boneWeights, model);
		ProcessScene(mutex, document, boneNames, skins);
		ProcessAnimations(mutex, document, boneNames, path, animations);
	}
	
}
