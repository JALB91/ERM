#pragma once

#include "erm/math/mat.h"

namespace erm {

struct InstanceData
{
	InstanceData(
		math::mat4 transform = math::mat4(1.0f),
		math::mat4 transformIT = math::mat4(1.0f))
		: mTransform(transform)
		, mTransformIT(transformIT)
	{}

	alignas(16) math::mat4 mTransform;
	alignas(16) math::mat4 mTransformIT;
};

} // namespace erm