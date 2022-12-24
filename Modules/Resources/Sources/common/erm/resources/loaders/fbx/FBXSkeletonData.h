#pragma once

#include <string>

namespace erm {

struct FbxSkeletonData
{
	FbxSkeletonData(
		const char* boneName,
		float boneWeight)
		: mBoneName(boneName)
		, mBoneWeight(boneWeight)
	{}

	const std::string mBoneName;
	const float mBoneWeight;
};

} // namespace erm
