#pragma once

#include <erm/utils/StringID.h>

#include <string_view>

namespace erm {

struct FbxSkeletonData
{
	FbxSkeletonData(
		std::string_view boneName,
		float boneWeight)
		: mBoneID(boneName)
		, mBoneWeight(boneWeight)
	{}

	const StringID mBoneID;
	const float mBoneWeight;
};

} // namespace erm
