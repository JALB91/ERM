#pragma once

#include "erm/math/mat.h"

#include "erm/utils/Tree.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <string>

namespace erm {
	
	typedef unsigned int BoneId;
	
	static const BoneId MAX_BONES = 100;
	
	struct Bone
	{
		Bone(
			const math::mat4& localTransform,
			const math::mat4& inverseBindTransform,
			const char* name
			)
			: mInverseBindTransform(inverseBindTransform)
			, mAnimatedTransform(glm::identity<math::mat4>())
			, mLocalTransform(localTransform)
			, mName(name)
		{}
		
		const math::mat4 mInverseBindTransform;
		math::mat4 mAnimatedTransform;
		math::mat4 mLocalTransform;
		std::string mName;
	};
	
	typedef Tree<BoneId, std::unique_ptr<Bone>> BonesTree;
	
}
