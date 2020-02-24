#pragma once

#include "math/mat.h"

#include <string>
#include <vector>

namespace erm {
	
	typedef int BoneId;
	
	struct Bone
	{
		const std::string mName;
		
		const BoneId mId;
		const BoneId mParentId;
		const std::vector<BoneId> mChildren;
		
		const math::mat4 mBindTransform;
		math::mat4 mAnimatedTransofrm;
	};
	
}
