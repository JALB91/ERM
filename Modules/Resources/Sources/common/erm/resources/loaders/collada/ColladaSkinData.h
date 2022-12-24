#pragma once

#include "erm/resources/models/VertexTypes.h"

#include <string>
#include <vector>

namespace erm {

struct ColladaSkinData
{
	std::vector<BoneIds> mBoneIds;
	std::vector<BoneWeights> mBoneWeights;
	std::vector<std::string> mBoneNames;
};

} // namespace erm
