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
	inline const mat4& getWorldTransform() const { return mWorldTransform; };
	inline const mat4& getLocalTransform() const { return mLocalTransform; };

	ERM_SENSIBLE_MEMBER(Translation, vec3, vec3(0.0f), UpdateDirtyMode::RECURSIVE);
	ERM_SENSIBLE_MEMBER(Rotation, vec3, vec3(0.0f), UpdateDirtyMode::RECURSIVE);
	ERM_SENSIBLE_MEMBER(Scale, vec3, vec3(1.0f), UpdateDirtyMode::RECURSIVE);

	inline void setTranslationX(float x)
	{
		if (mTranslation.x == x)
		{
			return;
		}
		mTranslation.x = x;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void setTranslationY(float y)
	{
		if (mTranslation.y == y)
		{
			return;
		}
		mTranslation.y = y;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void setTranslationZ(float z)
	{
		if (mTranslation.z == z)
		{
			return;
		}
		mTranslation.z = z;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void setRotationX(float x)
	{
		if (mRotation.x == x)
		{
			return;
		}
		mRotation.x = x;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void setRotationY(float y)
	{
		if (mRotation.y == y)
		{
			return;
		}
		mRotation.y = y;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void setRotationZ(float z)
	{
		if (mRotation.z == z)
		{
			return;
		}
		mRotation.z = z;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void setScaleX(float x)
	{
		if (mScale.x == x)
		{
			return;
		}
		mScale.x = x;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void setScaleY(float y)
	{
		if (mScale.y == y)
		{
			return;
		}
		mScale.y = y;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

	inline void setScaleZ(float z)
	{
		if (mScale.z == z)
		{
			return;
		}
		mScale.z = z;
		setDirty(true, UpdateDirtyMode::RECURSIVE);
	}

private:
	mat4 mWorldTransform = glm::identity<mat4>();
	mat4 mLocalTransform = glm::identity<mat4>();
};

} // namespace erm::ecs
