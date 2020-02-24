#pragma once

#include "math/mat.h"

namespace erm {
	
	typedef int BoneId;
	static const BoneId MAX_BONES = 50;
	static const BoneId INVALID_BONE = -1;
	
	struct Bone
	{
		const BoneId mId;
		
		const math::mat4 mBindTransform;
		const math::mat4 mInverseBindTransform;
		math::mat4 mAnimatedTransform;
	};
	
}
