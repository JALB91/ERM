#pragma once

#include <erm/math/mat.h>

#include <erm/utils/StringID.h>
#include <erm/utils/Tree.h>

#include <glm/gtc/matrix_transform.hpp>

#include <string_view>

namespace erm {

using BoneID = unsigned int;

static constexpr BoneID MAX_BONES = 256;

struct Bone
{
	Bone(
		const math::mat4& bindTransform,
		const math::mat4& inverseBindTransform,
		std::string_view name)
		: mBindTransform(bindTransform)
		, mInverseBindTransform(inverseBindTransform)
		, mAnimatedTransform(glm::identity<math::mat4>())
		, mLocalTransform(bindTransform)
		, mWorldTransform(glm::identity<math::mat4>())
		, mName(name)
	{}

	math::mat4 mBindTransform;
	math::mat4 mInverseBindTransform;
	math::mat4 mAnimatedTransform;
	math::mat4 mLocalTransform;
	math::mat4 mWorldTransform;
	StringID mName;
};

using BonesTree = Tree<BoneID, Bone>;

} // namespace erm
