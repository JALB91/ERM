#pragma once

#include <erm/math/Types.h>

#include <erm/utils/StringID.h>
#include <erm/utils/Tree.h>

#include <glm/gtc/matrix_transform.hpp>

#include <string_view>

namespace erm {

using BoneID = u16;

static constexpr BoneID MAX_BONES = 256;

struct Bone
{
	Bone(
		const mat4& bindTransform,
		const mat4& inverseBindTransform,
		std::string_view name)
		: mBindTransform(bindTransform)
		, mInverseBindTransform(inverseBindTransform)
		, mAnimatedTransform(glm::identity<mat4>())
		, mLocalTransform(bindTransform)
		, mWorldTransform(glm::identity<mat4>())
		, mName(name)
	{}

	mat4 mBindTransform;
	mat4 mInverseBindTransform;
	mat4 mAnimatedTransform;
	mat4 mLocalTransform;
	mat4 mWorldTransform;
	StringID mName;
};

using BonesTree = Tree<BoneID, Bone>;

} // namespace erm
