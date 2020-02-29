#pragma once

#include "math/mat.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	
	typedef unsigned int BoneId;
	static const BoneId MAX_BONES = 50;
	
	struct Bone
	{
		Bone(const math::mat4& inverseBindTransform)
			: mInverseBindTransform(inverseBindTransform)
			, mAnimatedTransform(glm::identity<math::mat4>())
		{}
		
		const math::mat4 mInverseBindTransform;
		math::mat4 mAnimatedTransform;
	};
	
}
