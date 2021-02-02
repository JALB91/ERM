#pragma once

#ifdef ERM_FBX_ENABLED

// clang-format off
#include <string>
// clang-format on

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

#endif