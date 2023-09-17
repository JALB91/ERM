#pragma once

#include "erm/ecs/Component.h"
#include "erm/ecs/EntityId.h"

#include <erm/math/Types.h>

#include <glm/gtc/matrix_transform.hpp>

namespace erm::ecs {
class TransformSystem;
}

namespace erm::ecs {

struct TransformComponent
{
	ERM_COMPONENT_DECL(Transform)

public:
	inline const mat4& GetWorldTransform() const { return mWorldTransform; };
	inline const mat4& GetLocalTransform() const { return mLocalTransform; };

	ERM_SENSIBLE_MEMBER(Translation, vec3, vec3(0.0f), UpdateDirtyMode::RECURSIVE);
	ERM_SENSIBLE_MEMBER(Rotation, vec3, vec3(0.0f), UpdateDirtyMode::RECURSIVE);
	ERM_SENSIBLE_MEMBER(Scale, vec3, vec3(1.0f), UpdateDirtyMode::RECURSIVE);

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
	mat4 mWorldTransform = glm::identity<mat4>();
	mat4 mLocalTransform = glm::identity<mat4>();
};

} // namespace erm::ecs
