#pragma once

#include "erm/ecs/Component.h"
#include "erm/ecs/EntityId.h"

#include "erm/math/mat.h"
#include "erm/math/vec.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm::ecs {
class TransformSystem;
}

namespace erm::ecs {

struct TransformComponent
{
	ERM_COMPONENT_DECL(Transform)

public:
	inline const math::mat4& GetWorldTransform() const { return mWorldTransform; };
	inline const math::mat4& GetLocalTransform() const { return mLocalTransform; };

	ERM_SENSIBLE_MEMBER(Translation, math::vec3, math::vec3(0.0f), UpdateDirtyMode::RECURSIVE);
	ERM_SENSIBLE_MEMBER(Rotation, math::vec3, math::vec3(0.0f), UpdateDirtyMode::RECURSIVE);
	ERM_SENSIBLE_MEMBER(Scale, math::vec3, math::vec3(1.0f), UpdateDirtyMode::RECURSIVE);

	inline void SetTranslationX(float x)
	{
		if (mTranslation.x == x)
			return;
		mTranslation.x = x;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void SetTranslationY(float y)
	{
		if (mTranslation.y == y)
			return;
		mTranslation.y = y;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void SetTranslationZ(float z)
	{
		if (mTranslation.z == z)
			return;
		mTranslation.z = z;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void SetRotationX(float x)
	{
		if (mRotation.x == x)
			return;
		mRotation.x = x;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void SetRotationY(float y)
	{
		if (mRotation.y == y)
			return;
		mRotation.y = y;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void SetRotationZ(float z)
	{
		if (mRotation.z == z)
			return;
		mRotation.z = z;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void SetScaleX(float x)
	{
		if (mScale.x == x)
			return;
		mScale.x = x;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void SetScaleY(float y)
	{
		if (mScale.y == y)
			return;
		mScale.y = y;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void SetScaleZ(float z)
	{
		if (mScale.z == z)
			return;
		mScale.z = z;
		SetDirty(true, UpdateDirtyMode::RECURSIVE);
	}

private:
	math::mat4 mWorldTransform = glm::identity<math::mat4>();
	math::mat4 mLocalTransform = glm::identity<math::mat4>();
};

} // namespace erm::ecs
