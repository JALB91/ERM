#include "erm/resources/loaders/collada/ColladaModelLoader.h"

#include "erm/resources/loaders/collada/ColladaAnimationsLoader.h"
#include "erm/resources/loaders/collada/ColladaGeometriesLoader.h"
#include "erm/resources/loaders/collada/ColladaLoaderUtils.h"
#include "erm/resources/loaders/collada/ColladaSceneLoader.h"
#include "erm/resources/loaders/collada/ColladaSkeletonLoader.h"
#include "erm/resources/loaders/collada/ColladaSkinData.h"

#include "erm/resources/ResourcesManager.h"

#include <tinyxml2.h>

#include <iostream>
#include <map>
#include <string>

using namespace tinyxml2;

namespace erm {

void ParseColladaModel(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	std::string_view path,
	Model& model,
	ResourcesManager& resourcesManager)
{
	tinyxml2::XMLDocument document;
	
	std::string sPath (path.data(), path.size());
	XMLError error = document.LoadFile(sPath.c_str());

	if (error != XMLError::XML_SUCCESS)
	{
		std::cout << "Error %d while reading dae file %s" << error << path << std::endl;
		return;
	}

	std::map<std::string, ColladaSkinData> skinsData;

	ProcessSkeleton(document, skinsData);
	ProcessGeometries(mutex, stop, document, model, resourcesManager, skinsData);
	ProcessScene(mutex, path, document, resourcesManager.GetSkins(), skinsData);
	ProcessAnimations(mutex, document, skinsData, path, resourcesManager.GetAnimations());
}

} // namespace erm
