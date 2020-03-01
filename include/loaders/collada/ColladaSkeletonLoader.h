#pragma once

#include "loaders/collada/ColladaSkinData.h"

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
