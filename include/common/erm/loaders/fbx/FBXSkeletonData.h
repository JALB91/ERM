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
			const math::mat4& transformMatrix,
			const math::mat4& matrix)
			: mBoneName(boneName)
			, mBoneWeight(boneWeight)
			, mTransformMatrix(transformMatrix)
			, mMatrix(matrix)
		{}

		const std::string mBoneName;
		const float mBoneWeight;
		const math::mat4 mTransformMatrix;
		const math::mat4 mMatrix;
	};

} // namespace erm

#endif