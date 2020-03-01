#pragma once

#include "math/mat.h"

#include "rendering/buffers/VertexData.h"

#include <string>
#include <vector>

namespace tinyxml2 {
	class XMLDocument;
}

namespace erm {
	
	void ProcessSkeleton(
		tinyxml2::XMLDocument& document,
		std::vector<BoneIds>& boneIds,
		std::vector<BoneWeights>& boneWeigths,
		std::vector<std::string>& boneNames
	);
	
}
