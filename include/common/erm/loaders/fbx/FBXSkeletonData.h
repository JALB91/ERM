#pragma once

#ifdef ERM_FBX_ENABLED

// clang-format off
#include "erm/math/mat.h"

#include <string>
// clang-format on

namespace erm {

	struct FbxSkeletonData
	{
		FbxSkeletonData(
			const char* boneName,
			float boneWeight,
			const math::mat4& localTransform,
			const math::mat4& inverseBindTransform)
			: mBoneName(boneName)
			, mBoneWeight(boneWeight)
			, mLocalTransform(localTransform)
			, mInverseBindTransform(inverseBindTransform)
		{}

		const std::string mBoneName;
		const float mBoneWeight;
		const math::mat4 mLocalTransform;
		const math::mat4 mInverseBindTransform;
	};

} // namespace erm

#endif