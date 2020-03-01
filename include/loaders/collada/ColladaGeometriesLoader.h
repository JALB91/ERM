#pragma once

#include "rendering/buffers/VertexData.h"

#include <atomic>
#include <mutex>
#include <vector>

namespace tinyxml2 {
	class XMLDocument;
}

namespace erm {
	
	class Model;
	
	extern void ProcessGeometries(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		tinyxml2::XMLDocument& document,
		const std::vector<BoneIds>& boneIds,
		const std::vector<BoneWeights>& boneWeights,
		Model& model
	);
	
}
