#pragma once

#include "math/mat.h"

namespace erm {
	
	typedef unsigned int BoneId;
	static const BoneId MAX_BONES = 50;
	
	struct Bone
	{
		Bone(BoneId id, const math::mat4& bindTransform, const math::mat4& inverseBindTransform)
			: mId(id)
			, mBindTransform(bindTransform)
			, mInverseBindTransform(inverseBindTransform)
			, mAnimatedTransform(bindTransform)
		{}
		
		const BoneId mId;
		
		const math::mat4 mBindTransform;
		const math::mat4 mInverseBindTransform;
		math::mat4 mAnimatedTransform;
	};
	
}
