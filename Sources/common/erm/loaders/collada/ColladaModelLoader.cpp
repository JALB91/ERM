#include "erm/loaders/collada/ColladaModelLoader.h"

#include "erm/loaders/collada/ColladaAnimationsLoader.h"
#include "erm/loaders/collada/ColladaGeometriesLoader.h"
#include "erm/loaders/collada/ColladaLoaderUtils.h"
#include "erm/loaders/collada/ColladaSceneLoader.h"
#include "erm/loaders/collada/ColladaSkeletonLoader.h"
#include "erm/loaders/collada/ColladaSkinData.h"

#include "erm/managers/ResourcesManager.h"

#include <tinyxml2.h>

#include <iostream>
#include <map>
#include <string>

using namespace tinyxml2;

namespace erm {

void ParseColladaModel(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	ResourcesManager& resourcesManager)
{
	tinyxml2::XMLDocument document;
	XMLError error = document.LoadFile(path);

	if (error != XMLError::XML_SUCCESS)
	{
		std::cout << "Error %d while reading dae file %s" << error << path << std::endl;
		return;
	}

	std::map<std::string, ColladaSkinData> skinsData;

	ProcessSkeleton(document, skinsData);
	ProcessGeometries(mutex, stop, document, model, skinsData);
	ProcessScene(mutex, path, document, resourcesManager.GetSkins(), skinsData);
	ProcessAnimations(mutex, document, skinsData, path, resourcesManager.GetAnimations());
}

} // namespace erm
