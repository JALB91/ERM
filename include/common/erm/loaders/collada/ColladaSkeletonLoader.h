#pragma once

#include "erm/loaders/collada/ColladaSkinData.h"

#include <map>

namespace tinyxml2 {
	class XMLDocument;
}

namespace erm {
	
	void ProcessSkeleton(
		tinyxml2::XMLDocument& document,
		std::map<std::string, ColladaSkinData>& skinsData
	);
	
}
