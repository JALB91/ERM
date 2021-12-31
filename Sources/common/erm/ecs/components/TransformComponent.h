#pragma once

#include "erm/ecs/EntityId.h"
#include "erm/ecs/IComponent.h"

#include "erm/math/mat.h"
#include "erm/math/vec.h"

#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace erm::ecs {
class TransformSystem;
}

namespace erm::ecs {

struct TransformComponent : public IComponent
{
public:
	typedef TransformSystem SYSTEM_TYPE;
	friend class TransformSystem;

public:
	TransformComponent(EntityId parent = {})
		: mParent(parent)
	{}

	// IComponent
	void SetDirty(bool isDirty) override;

	inline EntityId GetParent() const { return mParent; }
	inline const std::vector<EntityId>& GetChildren() const { return mChildren; }

	ERM_SENSIBLE_MEMBER(WorldTransform, math::mat4, glm::identity<math::mat4>());
	ERM_SENSIBLE_MEMBER(LocalTransform, math::mat4, glm::identity<math::mat4>());
	ERM_SENSIBLE_MEMBER(Translation, math::vec3, math::vec3(0.0f));
	ERM_SENSIBLE_MEMBER(Rotation, math::vec3, math::vec3(0.0f));
	ERM_SENSIBLE_MEMBER(Scale, math::vec3, math::vec3(1.0f));

	inline void SetTranslationX(float x)
	{
		if (mTranslation.x == x)
			return;
		mTranslation.x = x;
		SetDirty(true);
	}

	inline void SetTranslationY(float y)
	{
		if (mTranslation.y == y)
			return;
		mTranslation.y = y;
		SetDirty(true);
	}

	inline void SetTranslationZ(float z)
	{
		if (mTranslation.z == z)
			return;
		mTranslation.z = z;
		SetDirty(true);
	}

	inline void SetRotationX(float x)
	{
		if (mRotation.x == x)
			return;
		mRotation.x = x;
		SetDirty(true);
	}

	inline void SetRotationY(float y)
	{
		if (mRotation.y == y)
			return;
		mRotation.y = y;
		SetDirty(true);
	}

	inline void SetRotationZ(float z)
	{
		if (mRotation.z == z)
			return;
		mRotation.z = z;
		SetDirty(true);
	}

	inline void SetScaleX(float x)
	{
		if (mScale.x == x)
			return;
		mScale.x = x;
		SetDirty(true);
	}

	inline void SetScaleY(float y)
	{
		if (mScale.y == y)
			return;
		mScale.y = y;
		SetDirty(true);
	}

	inline void SetScaleZ(float z)
	{
		if (mScale.z == z)
			return;
		mScale.z = z;
		SetDirty(true);
	}

private:
	EntityId mParent;
	std::vector<EntityId> mChildren;
};

} // namespace erm::ecs
